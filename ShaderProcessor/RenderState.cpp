//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Shlwapi.h>
#include <istream>
#include <sstream>
#include <regex>

//////////////////////////////////////////////////////////////////////

CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

//////////////////////////////////////////////////////////////////////

std::map<string, Semantic> semantics;

//////////////////////////////////////////////////////////////////////

enum param_type
{
	bool_param = 0,
	inverse_bool_param = 1,
	enum_param = 2,
	float_param = 3,
	uint8_param = 4,
	int_param = 5
};

//////////////////////////////////////////////////////////////////////

std::map<param_type, char const *> param_names =
{
	{ bool_param, "bool" },
	{ inverse_bool_param, "bool" },
	{ enum_param, "enum" },
	{ float_param, "float" },
	{ uint8_param, "byte" },
	{ int_param, "int" }
};

//////////////////////////////////////////////////////////////////////

std::map<param_type, char const *> param_print_strings =
{
	{ bool_param, "%d" },
	{ inverse_bool_param, "%d" },
	{ float_param, "%f" },
	{ uint8_param, "%02x" },
	{ int_param, "%d" }
};

//////////////////////////////////////////////////////////////////////

char const *param_type_name(param_type t)
{
	auto f = param_names.find(t);
	return (f == param_names.end()) ? "??" : f->second;
}

//////////////////////////////////////////////////////////////////////

char const *param_printf_string(param_type t)
{
	auto f = param_print_strings.find(t);
	return (f == param_print_strings.end()) ? "" : f->second;
}

//////////////////////////////////////////////////////////////////////

using MAP = std::map < string, uint32 > ;

struct parameter
{
	char const *name;
	param_type type;
	void *target;	// the default is what's in here to start with (from D3D11_DEFAULT)
	MAP enum_list;	// empty if it's not an enum - this is lame...
};

//////////////////////////////////////////////////////////////////////
// Process key=value entries in #pragma token(key=value)s

std::map<param_type, std::function<void(string const &value, parameter const &param, uintptr offset)>> paramHandlerMap =
{
	//////////////////////////////////////////////////////////////////////
	// bool
	{
		bool_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			BOOL r = TRUE;
			if(strcmp(value.c_str(), "false") == 0)
			{
				r = FALSE;
			}
			else if(value.size() > 0 && strcmp(value.c_str(), "true") != 0)
			{
				emit_warning("Invalid value %s for boolean parameter", value.c_str());
			}
			*((BOOL *)((byte *)param.target + offset)) = r;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// inverse_bool
	{
		inverse_bool_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			BOOL r = FALSE;
			if(strcmp(value.c_str(), "false") == 0)
			{
				r = TRUE;
			}
			else if(value.size() > 0 && strcmp(value.c_str(), "true") != 0)
			{
				emit_warning("Invalid value %s for inverse boolean parameter", value.c_str());
			}
			*((BOOL *)((byte *)param.target + offset)) = r;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// enum
	{
		enum_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			for(auto &e : param.enum_list)
			{
				if(strcmp(value.c_str(), e.first.c_str()) == 0)
				{
					*((DWORD *)((byte *)param.target + offset)) = e.second;
					return;
				}
			}
			emit_error("Unknown enum value %s for %s\n", value.c_str(), param.name);
		}
	},

	//////////////////////////////////////////////////////////////////////
	// float
	{
		float_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			char *end;
			float f = strtof(value.c_str(), &end);
			*((float *)((byte *)param.target + offset)) = f;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// uint8
	{
		uint8_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			char *end;
			long f = strtol(value.c_str(), &end, 10);
			if(f < 0 || f > 255)
			{
				emit_warning("Value %d out of range (0-255), truncated", f);
			}
			*((uint8 *)((byte *)param.target + offset)) = (uint8)f;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// int
	{
		int_param, [] (string const &value, parameter const &param, uintptr offset)
		{
			long l = strtol(value.c_str(), null, 10);
			*((int *)((byte *)param.target + offset)) = l;
		}
	},
};

//////////////////////////////////////////////////////////////////////

MAP depth_write_map =
{
	{ "enabled", D3D11_DEPTH_WRITE_MASK_ALL },
	{ "disabled", D3D11_DEPTH_WRITE_MASK_ZERO }
};

//////////////////////////////////////////////////////////////////////

MAP comparison_map =
{
	{ "less", D3D11_COMPARISON_LESS },
	{ "equal", D3D11_COMPARISON_EQUAL },
	{ "less_equal", D3D11_COMPARISON_LESS_EQUAL },
	{ "greater", D3D11_COMPARISON_GREATER },
	{ "not_equal", D3D11_COMPARISON_NOT_EQUAL },
	{ "greater_equal", D3D11_COMPARISON_GREATER_EQUAL },
	{ "always", D3D11_COMPARISON_ALWAYS }
};

//////////////////////////////////////////////////////////////////////

MAP stencil_op_map =
{
	{ "keep", D3D11_STENCIL_OP_KEEP },
	{ "zero", D3D11_STENCIL_OP_ZERO },
	{ "replace", D3D11_STENCIL_OP_REPLACE },
	{ "incr_sat", D3D11_STENCIL_OP_INCR_SAT },
	{ "decr_sat", D3D11_STENCIL_OP_DECR_SAT },
	{ "invert", D3D11_STENCIL_OP_INVERT },
	{ "incr", D3D11_STENCIL_OP_INCR },
	{ "decr", D3D11_STENCIL_OP_DECR }
};

//////////////////////////////////////////////////////////////////////

MAP culling_mode_map =
{
	{ "none", D3D11_CULL_NONE },
	{ "front", D3D11_CULL_FRONT },
	{ "back", D3D11_CULL_BACK }
};

//////////////////////////////////////////////////////////////////////

MAP culling_order_map =
{
	{ "cw", FALSE },
	{ "ccw", TRUE }
};

//////////////////////////////////////////////////////////////////////

MAP fill_mode_map =
{
	{ "solid", D3D11_FILL_SOLID },
	{ "wireframe", D3D11_FILL_WIREFRAME }
};

//////////////////////////////////////////////////////////////////////

MAP blend_op_map =
{
	{ "add", D3D11_BLEND_OP_ADD },
	{ "subtract", D3D11_BLEND_OP_SUBTRACT },
	{ "rev_subtract", D3D11_BLEND_OP_REV_SUBTRACT },
	{ "min", D3D11_BLEND_OP_MIN },
	{ "max", D3D11_BLEND_OP_MAX }
};

//////////////////////////////////////////////////////////////////////

MAP blend_map =
{
	{ "zero", D3D11_BLEND_ZERO },
	{ "one", D3D11_BLEND_ONE },
	{ "src_color", D3D11_BLEND_SRC_COLOR },
	{ "inv_src_color", D3D11_BLEND_INV_SRC_COLOR },
	{ "src_alpha", D3D11_BLEND_SRC_ALPHA },
	{ "inv_src_alpha", D3D11_BLEND_INV_SRC_ALPHA },
	{ "dest_alpha", D3D11_BLEND_DEST_ALPHA },
	{ "inv_dest_alpha", D3D11_BLEND_INV_DEST_ALPHA },
	{ "dest_color", D3D11_BLEND_DEST_COLOR },
	{ "inv_dest_color", D3D11_BLEND_INV_DEST_COLOR },
	{ "src_alpha_sat", D3D11_BLEND_SRC_ALPHA_SAT },
	{ "blend_factor", D3D11_BLEND_BLEND_FACTOR },
	{ "inv_blend_factor", D3D11_BLEND_INV_BLEND_FACTOR },
	{ "src1_color", D3D11_BLEND_SRC1_COLOR },
	{ "inv_src1_color", D3D11_BLEND_INV_SRC1_COLOR },
	{ "src1_alpha", D3D11_BLEND_SRC1_ALPHA },
	{ "inv_src1_alpha", D3D11_BLEND_INV_SRC1_ALPHA }
};

//////////////////////////////////////////////////////////////////////

struct param_vec
{
	char const *description;
	uint32 max_index;
	uint32 offset_size;
	vector<parameter> params;
};

std::map<string, param_vec> params =
{
	//////////////////////////////////////////////////////////////////////
	// DEPTH_STENCIL_STATE

	{
		"depth",
		{
			"Enable/Disable depth buffer reading & writing",
			0, 0,
			{
				{ "enabled", bool_param, &depthStencilDesc.DepthEnable, {} },
				{ "disabled", inverse_bool_param, &depthStencilDesc.DepthEnable, {} },
				{ "write", enum_param, &depthStencilDesc.DepthWriteMask, depth_write_map },
				{ "comparison_func", enum_param, &depthStencilDesc.DepthFunc, comparison_map }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"stencil",
		{
			"Enable/Disable stencil and set read/write masks",
			0, 0,
			{
				{ "enabled", bool_param, &depthStencilDesc.StencilEnable, {} },
				{ "disabled", inverse_bool_param, &depthStencilDesc.StencilEnable, {} },
				{ "read_mask", uint8_param, &depthStencilDesc.StencilReadMask, {} },
				{ "write_mask", uint8_param, &depthStencilDesc.StencilWriteMask, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"front_stencil",
		{
			"Front Stencil operations and comparison function",
			0, 0,
			{
				{ "fail_op", enum_param, &depthStencilDesc.FrontFace.StencilFailOp, stencil_op_map },
				{ "depth_fail_op", enum_param, &depthStencilDesc.FrontFace.StencilDepthFailOp, stencil_op_map },
				{ "pass_op", enum_param, &depthStencilDesc.FrontFace.StencilPassOp, stencil_op_map },
				{ "comparison_func", enum_param, &depthStencilDesc.FrontFace.StencilFunc, comparison_map }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"back_stencil",
		{
			"Back Stencil operations and comparison function",
			0, 0,
			{
				{ "fail_op", enum_param, &depthStencilDesc.BackFace.StencilFailOp, stencil_op_map },
				{ "depth_fail_op", enum_param, &depthStencilDesc.BackFace.StencilDepthFailOp, stencil_op_map },
				{ "pass_op", enum_param, &depthStencilDesc.BackFace.StencilPassOp, stencil_op_map },
				{ "comparison_func", enum_param, &depthStencilDesc.BackFace.StencilFunc, comparison_map }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////
	// RASTERIZER_STATE

	{
		"culling",
		{
			"Culling mode & winding order (specifies the order of front-facing triangles)",
			0, 0,
			{
				{ "mode", enum_param, &rasterizerDesc.CullMode, culling_mode_map },
				{ "order", enum_param, &rasterizerDesc.FrontCounterClockwise, culling_order_map }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"fill",
		{
			"Triangle fill mode",
			0, 0,
			{
				{ "mode", enum_param, &rasterizerDesc.FillMode, fill_mode_map }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"depth_bias",
		{
			"Depth Bias values",
			0, 0,
			{
				{ "bias", int_param, &rasterizerDesc.DepthBias, {} },
				{ "clamp", float_param, &rasterizerDesc.DepthBiasClamp, {} },
				{ "slope_scaled_bias", float_param, &rasterizerDesc.SlopeScaledDepthBias, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"depth_clip",
		{
			"Depth Clipping enable/disable",
			0, 0,
			{
				{ "enabled", bool_param, &rasterizerDesc.DepthClipEnable, {} },
				{ "disabled", inverse_bool_param, &rasterizerDesc.DepthClipEnable, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"scissor",
		{
			"Scissor enable/disable",
			0, 0,
			{
				{ "enabled", bool_param, &rasterizerDesc.ScissorEnable, {} },
				{ "disabled", inverse_bool_param, &rasterizerDesc.ScissorEnable, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"multisample",
		{
			"Multisample enable/disable",
			0, 0,
			{
				{ "enabled", bool_param, &rasterizerDesc.MultisampleEnable, {} },
				{ "disabled", inverse_bool_param, &rasterizerDesc.MultisampleEnable, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"anti_aliased_line",
		{
			"Anti-aliased line drawing enable/disable",
			0, 0,
			{
				{ "enabled", bool_param, &rasterizerDesc.AntialiasedLineEnable, {} },
				{ "disabled", inverse_bool_param, &rasterizerDesc.AntialiasedLineEnable, {} }
			}
		}
	},

	//////////////////////////////////////////////////////////////////////
	// BLEND_STATE
	{
		"blend",
		{
			"Blend mode and operations",
			8, sizeof(D3D11_RENDER_TARGET_BLEND_DESC),
			{
				{ "enabled", bool_param, &blendDesc.RenderTarget[0].BlendEnable, {} },
				{ "disabled", inverse_bool_param, &blendDesc.RenderTarget[0].BlendEnable, {} },
				{ "op", enum_param, &blendDesc.RenderTarget[0].BlendOp, blend_op_map },
				{ "alpha_op", enum_param, &blendDesc.RenderTarget[0].BlendOpAlpha, blend_op_map },
				{ "src", enum_param, &blendDesc.RenderTarget[0].SrcBlend, blend_map },
				{ "dest", enum_param, &blendDesc.RenderTarget[0].DestBlend, blend_map },
				{ "alpha_src", enum_param, &blendDesc.RenderTarget[0].SrcBlendAlpha, blend_map },
				{ "alpha_dest", enum_param, &blendDesc.RenderTarget[0].DestBlendAlpha, blend_map }
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////

char const *bool_vals[] =
{
	"false", "true"
};

//////////////////////////////////////////////////////////////////////
// Auto-deleting temp file(name)

struct TempFile
{
	tstring filename;

	TempFile(tchar const *prefix)
	{
		tchar tempPath[MAX_PATH + 1];
		tchar tempFile[MAX_PATH + 1];
		GetTempPath(_countof(tempPath), tempPath);
		GetTempFileName(tempPath, prefix, 0, tempFile);
		filename = tempFile;
	}

	~TempFile()
	{
		DeleteFile(filename.c_str());
	}

	operator tchar const *()
	{
		return filename.c_str();
	}
};

//////////////////////////////////////////////////////////////////////
// Run a command (CL.EXE, say...)

bool Run(tchar const *exe, tstring &params, tchar const *env = null, bool wait = false, DWORD *exitCode = null)
{
	vector<tchar> commandBuffer;
	commandBuffer.reserve(params.size() + 1);
	commandBuffer.resize(params.size());
	std::copy(params.begin(), params.end(), commandBuffer.begin());
	commandBuffer.push_back(0);
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	DWORD exitCodeLocal;
	if(CreateProcess(exe, commandBuffer.data(), null, null, true, CREATE_NO_WINDOW | DETACHED_PROCESS, (void *)env, null, &si, &pi) == 0)
	{
		tstring err = Win32ErrorMessage(GetLastError());
		emit_error(Format("Error running %s %s : %s", exe != null ? exe : "", commandBuffer.data(), err.c_str()).c_str());
		return false;
	}
	if(wait)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitCodeLocal);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		WaitForInputIdle(pi.hProcess, 1000);
	}
	if(exitCode != null)
	{
		*exitCode = exitCodeLocal;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////
// Printf to a diskfile

void PutS(DiskFile &d, char const *s, ...)
{
	va_list v;
	va_start(v, s);
	string st = Format_V(s, v);
	d.Write(st.data(), (uint32)st.size());
}

//////////////////////////////////////////////////////////////////////
// Show the pragma_options

void OutputPragmaDocs()
{
	TempFile t("PRD");
	DiskFile d;
	if(d.Create(t, DiskFile::Overwrite) == S_OK)
	{
		PutS(d, R"(<html>
<body style="font-family:verdana; font-size:small;">
    <style>
        .lightBackground
        {
            background-color:#eee;
        }
        .banner
        {
            background-color:#ccc;
            line-height:1em;
            border:1px solid;
            padding:0 0 0 1em;
        }
        .nomargin
        {
            margin:0;
            padding:0;
        }
        .leftPadding
        {
            padding-left:1em;
        }
        .rightPadding
        {
            padding-right:1em;            
        }
        .rightMargin
        {
            margin-right:1em;
        }
        .leftMargin
        {
            margin-left:1em;
        }
        .vPadding
        {
            padding-top:1em;
            padding-bottom:1em;
        }
        .tPadding
        {
            padding-top:1em;
        }
        .bPadding
        {
            padding-bottom:1em;
        }
        .code
        {
            font-family:courier;
        }
        .outlined
        {
            border:1px solid;
        }
        table
        {
            border-spacing:0;
            border-collapse:collapse;
            font-size:small;
            vertical-align:top;
            margin: 0;
            padding: 0;
        }
        tr
        {
            border:solid 1px;
            background-color:#d8d8d8;
            font-weight:bold
        }
        td
        {
            padding:2px 10px 2px 2px;
            border:1px solid;
            margin:0;
        }
        tr.transparent
        {
            background-color:transparent;
        }
        td.header
        {
            font-weight:bold;
        }
        td.data
        {
            vertical-align: top;
            font-weight:normal;
        }
        emboldened
        {
            font-weight:bold;
            font-style:italic;
            text-decoration: underline;
        }
    </style>
    <div class="banner">
        <h2>ShaderProcessor</h2>
    </div>
    <div class="leftPadding">
        <h3>Pragma Options</h3>
        <p>Pragmas must start on the 1st column of a line and cannot contain line breaks.</p>
        <p>They're specified like this: </p>
        <p class="code">#pragma name(options)</p>
        <p>See below for the list of pragma names.</p>
        <p>The blend pragma must specify one or more optional indices (all channels specified will be assigned).</p>
        <p>Options are a series of name = value settings.The default value for enum types is emphasized.</p>
        <p>bool options can be specified as 'enabled' or 'disabled' or 'enabled = [true | false]' or 'disabled = [true | false].'</p>
    </div>
)");

		for(auto &token : params)
		{
			string index = (token.second.max_index > 0) ? Format("index %d-%d,", 0, token.second.max_index - 1) : "";

			PutS(d, R"(
    <div class="outlined">
        <div class="nomargin lightbackground">
            <h3 class="tPadding nomargin leftMargin">%s</h3>
        </div>
        <div class="lightBackground nomargin vPadding">
            <div class="leftMargin rightMargin">
                <h4 class="nomargin bPadding">#pragma %s(%soptions)</h4>
                <table>
                    <tr>
                        <td class="header">Option</td>
                        <td class="header">Input</td>
                    </tr>)", token.second.description, token.first.c_str(), index.c_str());

			for(auto &p : token.second.params)
			{
				if(p.type != inverse_bool_param)
				{
					string line;
					switch(p.type)
					{
						case enum_param:
							{
								char const *sep = "";
								for(auto &e : p.enum_list)
								{
									char const *tclass = "normal";
									if(e.second == *((uint32 *)p.target))
									{
										tclass = "emboldened";
									}
									line += sep;
									if(e.second == *((uint32 *)p.target))
									{
										line += "<em><strong><u>";
									}
									line += e.first;
									if(e.second == *((uint32 *)p.target))
									{
										line += "</u></strong></em>";
									}
									sep = " ";
								}
							}
							break;
						case bool_param:
							line = Format("%s (default = %s)", param_type_name(p.type), bool_vals[(*((BOOL *)p.target)) & 1]);
							break;
						case uint8_param:
							line = Format("%s (default = 0x%s)", param_type_name(p.type), Format(param_printf_string(p.type), *((uint8 *)p.target)).c_str());
							break;
						case float_param:
							line = Format("%s (default = %s)", param_type_name(p.type), Format(param_printf_string(p.type), *((float *)p.target)).c_str());
							break;
						case int_param:
							line = Format("%s (default = %s)", param_type_name(p.type), Format(param_printf_string(p.type), *((int32 *)p.target)).c_str());
							break;
					}
					PutS(d, R"(
                    <tr class="transparent">
                        <td class="header">%s</td>
                        <td class="data">%s</td>
                    </tr>)", p.name, line.c_str());
				}
			}
			PutS(d, R"(
                </table>
                <p></p>
            </div>
        </div>
    </div>
    <p></p>)");
		}
		PutS(d, R"(
</body>
</html>
)");

		d.Close();
		string n = SetExtension(t.filename.c_str(), ".html");
		MoveFile(t.filename.c_str(), n.c_str());
		Run(null, Format("cmd.exe /C \"start %s\"", n.c_str()));
	}
}

//////////////////////////////////////////////////////////////////////
// Get the path to CL.EXE

tstring GetCLPath()
{
	static tstring vcPath;
	static bool got = false;
	if(!got)
	{
		got = true;
		tstring version = "12.0";
		if(RegistryKey::GetString(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7"), version.c_str(), vcPath) == 0 ||
		   RegistryKey::GetString(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\VisualStudio\\SxS\\VC7"), version.c_str(), vcPath) == 0 ||
		   RegistryKey::GetString(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7"), version.c_str(), vcPath) == 0 ||
		   RegistryKey::GetString(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\SxS\\VC7"), version.c_str(), vcPath) == 0)
		{
			// Success!
		}
	}
	return vcPath;
}

//////////////////////////////////////////////////////////////////////
// Add a folder to the PATH environment variable

tstring AddToPath(tchar const *dir)
{
	tstring output;
	tchar const *env = GetEnvironmentStrings();
	tchar const *cur = env;
	while(*cur)
	{
		tstring line(cur, cur + _tcslen(cur) + 1);
		output = output + line;
		if(_tcsncmp(cur, TEXT("PATH="), 5) == 0)
		{
			output.append(cur + 5, cur + _tcslen(cur));
			output.append(TEXT(";"));
			output.append(dir);
		}
		cur += _tcslen(cur) + 1;
	}
	return output;
}

//////////////////////////////////////////////////////////////////////
// Preprocess using VC preprocessor, then scan for renderstate #pragmas [and semantic declarations?]

// pragma:
// ^#pragma\s*(\w+)\s*\((.*)\)\s*$

// semantic declaration:
// .*\:\s*semantic\s*:\s*\(\s*\"(.+)\"\s*\)

// name value pairs:
// (\w+)(\s*=\s*(\w+)|,)?

//////////////////////////////////////////////////////////////////////

using regex_iter = std::regex_iterator<tstring::iterator>;

static std::regex nameval_regex(R"((\w+)(\s*=\s*([_A-Za-z0-9\-\+\.]+)|,)?)");

//////////////////////////////////////////////////////////////////////

void GetNameValueMap(string &valstr, std::map<string, string> &keyVals)
{
	keyVals.clear();
	regex_iter rend;
	regex_iter a(valstr.begin(), valstr.end(), nameval_regex);
	while(a != rend)
	{
		auto &matches = *a;
		if(matches[1].matched)
		{
			string value = matches[3].matched ? matches[3].str() : "true";
			keyVals[matches[1].str()] = value;
		}
		++a;
	}
}

//////////////////////////////////////////////////////////////////////

bool GetFrom(std::map<string, string> keyVals, string const &str, string &result)
{
	auto f = keyVals.find(str);
	if(f != keyVals.end())
	{
		result = f->second;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

bool Semantic::Set(string const &type_name, string const &instances_str, string const &stream_str)
{
	type = Invalid_type;
	nativeFormat = GetDXGIDescriptorFromName(type_name.c_str());
	if(nativeFormat == null)
	{
		type = StorageTypeFromName(type_name.c_str());
	}

	if(!StrToIntEx(stream_str.c_str(), 0, &stream) || stream < 0) // TODO: check upper limit
	{
		emit_error("Bad stream");
		return false;
	}

	if(instances_str.empty())
	{
		instanced = false;
	}
	else
	{
		instanced = true;
		if(!StrToIntEx(instances_str.c_str(), 0, &instances) || instances < 1) // TODO: check upper limit
		{
			emit_error("Bad instance count");
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////

static std::regex pragma_regex(R"(^#\s*pragma\s+(\w+)\s*\((.*)\))");
static std::regex semantic_regex(R"((.*?)(\w+)\s*\:\s*(semantic)\s*:\s*\(\s*(.*)(\s*\))\s*;)"); // float3 position: semantic:(type=byte);

uint ScanMaterialOptions(tchar const *filename, string &output)
{
	output.clear();

	Error::current_line = 1;

	TempFile tempFile(TEXT("SOB"));
	TempFile errorFile(TEXT("SER"));

	tstring newEnv = AddToPath(GetCLPath().c_str());

	// run CL.EXE /EP /P on it (just preprocess)
	DWORD exitCode;
	if(!Run(null, Format(TEXT("\"%sbin\\cl.exe\" /EP /P \"%s\" /Fi\"%s\" /nologo"), GetCLPath().c_str(), filename, (tchar const *)tempFile), newEnv.data(), true, &exitCode))
	{
		return err_compilerproblem;
	}
	if(exitCode != 0)
	{
		return err_compilerproblem;
	}

	// load the output
	MemoryFile file;
	if(!LoadFile(tempFile, file))
	{
		return err_compilerproblem;
	}

	// scan it for #pragmas

	using keyvals = std::map < string, string > ;
	using paramList = std::map < string, keyvals > ;

	vector<string> lines;
	std::istringstream str((char *)file.ptr);
	string line;
	Error::current_line = 1;
	while(std::getline(str, line))
	{
		bool consume_line = false;

		std::cmatch m;
		if(regex_search(line.c_str(), m, pragma_regex) && m[1].matched)
		{
			string pragma = m[1].str();

			auto p = params.find(pragma);
			if(p != params.end())
			{
				consume_line = true;

				string valstr = m[2].str();
				if(!m[2].matched)
				{
					emit_warning("malformed pragma");
					continue;
				}
				else
				{
					keyvals keyVals;
					GetNameValueMap(valstr, keyVals);

					// is it an indexed parameter (eg blend 0-7)

					uint max_index = p->second.max_index;
					uint offset_size = p->second.offset_size;
					vector<uint> indices;

					// look for an index - an integer (#=true) in keyvals
					// if we don't find one and max_index > 0, malformed pragma
					// if we find more than one, warn
					int got_index = 0;

					if(max_index > 0)
					{
						for(uint idx = 0; idx < max_index; ++idx)
						{
							auto kv = keyVals.find(Format("%d", idx));
							if(kv != keyVals.end())
							{
								indices.push_back(idx);
								keyVals.erase(kv);
								++got_index;
							}
						}
					}

					if(got_index == 0)
					{
						indices.push_back(0);
					}

					for(auto &keyval : keyVals)
					{
						string keyname = keyval.first;
						string keyValue = keyval.second;

						bool valid = false;

						for(auto &option : p->second.params)
						{
							if(strcmp(keyname.c_str(), option.name) == 0)
							{
								valid = true;
								auto fnc = paramHandlerMap.find(option.type);
								if(fnc == paramHandlerMap.end())
								{
									emit_error("Unknown parameter type?");
								}
								else
								{
									for(auto index : indices)
									{
										fnc->second(keyValue, option, index * p->second.offset_size);
									}
								}
								break;
							}
						}
						if(!valid)
						{
							emit_error("Unknown key name %s", keyname.c_str());
							return err_unknownkey;
						}
					}
				}
			}
		}
		else if(regex_search(line.c_str(), m, semantic_regex) && m[1].matched && m[2].matched && m[3].matched && m[5].matched)
		{
			keyvals keyVals;
			GetNameValueMap(m[4].str(), keyVals);

			string type = "";
			GetFrom(keyVals, "type", type);

			string stream = "0";
			GetFrom(keyVals, "stream", stream);

			string instances;
			GetFrom(keyVals, "instances", instances);

			string name;
			GetFrom(keyVals, "name", name);

			if(name.empty())
			{
				name = m[2].str();	// just use the name of the member in the struct
			}

			Semantic s;

			if(!s.Set(type, instances, stream))
			{
				return E_INVALIDARG;
			}

			// work out what bit to replace
			auto start = m.position();
			auto end = start + m.length();

			auto start2 = start + std::distance(m[1].first, m[3].first);
			auto end2 = start + std::distance(m[1].first, m[5].second);

			// replace the semantic declaration with the semantic name
			string post = line.substr(end2);
			string pre = line.substr(0, start2);
			line = pre + name + post;

			// remember for later after shader compilation
			auto f = semantics.find(name);
			if(f == semantics.end())
			{
				semantics[name] = s;
			}
			else
			{
				emit_error("Duplicate semantic name");
				return E_INVALIDARG;
			}

		}
		++Error::current_line;
		if(!consume_line)
		{
			output.append(line);
		}
	}
	return success;
}

