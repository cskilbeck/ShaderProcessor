//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <istream>
#include <sstream>
#include <regex>

//////////////////////////////////////////////////////////////////////

CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

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
			char *end;
			long l = strtol(value.c_str(), &end, 10);
			*((int *)((byte *)param.target + offset)) = l;
		}
	},
};

//////////////////////////////////////////////////////////////////////

MAP depth_write_map =
{
	{ "enabled", D3D11_DEPTH_WRITE_MASK_ALL },
	{ "on", D3D11_DEPTH_WRITE_MASK_ALL },

	{ "disabled", D3D11_DEPTH_WRITE_MASK_ZERO },
	{ "off", D3D11_DEPTH_WRITE_MASK_ZERO }
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

void OutputPragmaDocs()
{
	// reet
	// map<string, vector<parameter>>

	for(auto &token : params)
	{
		string index = "";
		if(token.second.max_index > 0)
		{
			index = Format("index %d-%d,", 0, token.second.max_index - 1);
		}
		printf("#pragma %s(%soptions)\n", token.first.c_str(), index.c_str());
		for(auto &p : token.second.params)
		{
			printf("  %s = ", p.name);
			switch(p.type)
			{
				case enum_param:
				{
					char sep = '[';
					for(auto &e : p.enum_list)
					{
						printf("%c%s", sep, e.first.c_str());
						if(e.second == *((uint32 *)p.target))
						{
							printf("*");
						}
						sep = '|';
					}
					printf("]\n");
				}
				break;
				case bool_param:
					printf("%s (default = %s)\n", param_type_name(p.type), bool_vals[(*((BOOL *)p.target)) & 1]);
					break;
				case inverse_bool_param:
					printf("%s (default = %s)\n", param_type_name(p.type), bool_vals[1 - ((*((BOOL *)p.target)) & 1)]);
					break;
				case uint8_param:
					printf("%s (default = 0x%s)\n", param_type_name(p.type), Format(param_printf_string(p.type), *((uint8 *)p.target)).c_str());
					break;
				case float_param:
					printf("%s (default = %s)\n", param_type_name(p.type), Format(param_printf_string(p.type), *((float *)p.target)).c_str());
					break;
				case int_param:
					printf("%s (default = %s)\n", param_type_name(p.type), Format(param_printf_string(p.type), *((int32 *)p.target)).c_str());
					break;
			}
		}
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
// Run a command (CL.EXE, say...)

bool Run(tchar const *exe, tstring &params, tchar const *env, DWORD &exitCode)
{
	vector<tchar> commandBuffer;
	commandBuffer.resize(params.size());
	std::copy(params.begin(), params.end(), commandBuffer.begin());
	commandBuffer.push_back(0);
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	if(CreateProcess(exe, commandBuffer.data(), null, null, true, 0, (void *)env, null, &si, &pi) != 0)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitCode);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	tstring err = Win32ErrorMessage(GetLastError());
	emit_error(Format("Error running CL.EXE: %s", err.c_str()).c_str());
	return false;
}

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
// Preprocess using VC preprocessor, then scan for renderstate #pragmas

uint ScanMaterialOptions(tchar const *filename)
{
	Error::current_line = 1;

	TempFile tempFile(TEXT("SOB"));
	TempFile errorFile(TEXT("SER"));

	tstring newEnv = AddToPath(GetCLPath().c_str());

	// run CL.EXE /EP /P on it (just preprocess)
	DWORD exitCode;
	if(!Run(null, Format(TEXT("\"%sbin\\cl.exe\" /EP /P \"%s\" /Fi\"%s\" /nologo"), GetCLPath().c_str(), filename, (tchar const *)tempFile), newEnv.data(), exitCode))
	{
		return err_compilerproblem;
	}
	if(exitCode != 0)
	{
		return err_compilerproblem;
	}
	// load the output
	FileResource file(tempFile);
	if(!file.IsValid())
	{
		return err_compilerproblem;
	}

	// scan it for #pragmas
	std::regex rgx(R"(^#\s*pragma\s+(.+)\((.*)\))");

	using keyvals = std::map < string, string > ;
	using paramList = std::map < string, keyvals > ;

	vector<string> lines;
	std::istringstream str((char *)file.Data());
	string line;
	Error::current_line = 1;
	while(std::getline(str, line))
	{
		std::cmatch m;
		if(regex_search(line.c_str(), m, rgx))
		{
			string token = m[1].str();

			// check if token is one we're interested in

			string valstr = m[2].str();
			vector<string> values;
			tokenize(valstr, values, ",");
			keyvals kvalues;
			for(auto &i : values)
			{
				vector<string> nameval;
				tokenize(i, nameval, "=");
				if(nameval.size() == 1)
				{
					kvalues[Trim(nameval[0])] = "true";
				}
				else if(nameval.size() == 2)
				{
					kvalues[Trim(nameval[0])] = Trim(nameval[1]);
				}
				else
				{
					// Harumph
					emit_error("Malformed Pragma");
					return err_malformedpragma;
				}
			}
			string paramName = token;
			keyvals &keyVals = kvalues;

			auto p = params.find(paramName);
			if(p == params.end())
			{
				emit_warning("unknown pragma: %s", paramName.c_str());
				continue;
			}

			uint max_index = p->second.max_index;
			uint offset_size = p->second.offset_size;
			vector<uint> indices;

			// look for an index - an integer (#=true) in keyvals
			// if we don't find one and max_index > 0, malformed pragma
			// if we find more than one, warn
			if(max_index > 0)
			{
				bool got_index = false;
				for(uint idx = 0; idx < max_index; ++idx)
				{
					auto kv = keyVals.find(Format("%d", idx));
					if(kv != keyVals.end())
					{
						indices.push_back(idx);
						keyVals.erase(kv);
						got_index = true;
					}
				}
				if(!got_index)
				{
					emit_error("Missing index");
					return err_malformedpragma;
				}
			}
			else
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
		++Error::current_line;
	}
	return success;
}

