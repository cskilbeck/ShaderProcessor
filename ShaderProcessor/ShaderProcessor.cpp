//////////////////////////////////////////////////////////////////////
// deal with multiple render targets
// enable mode where bytecode not embedded (somehow!?)

#include "stdafx.h"
#include <istream>
#include <sstream>
#include <regex>

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

using option::Option;
vector<Option> options;

//////////////////////////////////////////////////////////////////////

struct ShaderArgType
{
	int arg;
	ShaderType type;
};

//////////////////////////////////////////////////////////////////////

ShaderArgType shaderTypes[] =
{
	{ VERTEX_MAIN, ShaderType::Vertex },
	{ PIXEL_MAIN, ShaderType::Pixel },
	{ GEOMETRY_MAIN, ShaderType::Geometry }
};

//////////////////////////////////////////////////////////////////////

uint32 optionMap[][2]=
{
	{ DISABLE_OPTIMIZATION,	D3DCOMPILE_SKIP_OPTIMIZATION		},
	{ INCLUDE_DEBUG_INFO,	D3DCOMPILE_DEBUG					},
	{ ERROR_ON_WARNING,		D3DCOMPILE_WARNINGS_ARE_ERRORS		},
	{ PACK_MATRIX_ROW_MAJOR,D3DCOMPILE_PACK_MATRIX_ROW_MAJOR	},
	{ PARTIAL_PRECISION,	D3DCOMPILE_PARTIAL_PRECISION		},
	{ NO_PRESHADER,			D3DCOMPILE_NO_PRESHADER				},
	{ AVOID_FLOW_CONTROL,	D3DCOMPILE_AVOID_FLOW_CONTROL		},
	{ PREFER_FLOW_CONTROL,	D3DCOMPILE_PREFER_FLOW_CONTROL		},
	{ ENABLE_STRICTNESS,	D3DCOMPILE_ENABLE_STRICTNESS		},
	{ IEEE_STRICTNESS,		D3DCOMPILE_IEEE_STRICTNESS			},
	{ RESOURCES_MAY_ALIAS,	D3DCOMPILE_RESOURCES_MAY_ALIAS		}
};

uint32 GetFlags()
{
	uint32 flag = 0;
	for(int i = 0; i < _countof(optionMap); ++i)
	{
		if(options[optionMap[i][0]])
		{
			flag |= optionMap[i][1];
		}
	}
	if(options[OPTIMIZATION_LEVEL])
	{
		switch(atoi(options[OPTIMIZATION_LEVEL].arg))
		{
			case 0: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL0; break;
			case 1: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL1; break;
			case 2: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL2; break;
			case 3: flag |= D3DCOMPILE_OPTIMIZATION_LEVEL3; break;
		}
	}
	return flag;
}

//////////////////////////////////////////////////////////////////////

std::map<ShaderType, HLSLShader *> shaders;

//////////////////////////////////////////////////////////////////////

bool CompileFile(string &file, char const *filename, char const *mainFunction, char const *shaderModel, ShaderType type)
{
	ShaderTypeDesc const *desc = ShaderTypeDescFromType(type);
	if(desc == null)
	{
		fprintf(stderr, "Unknown shader type: %d\n", type);
		return false;
	}
	string refname = ToLower(desc->refName);
	string shader = Format("%s_%s", refname.c_str(), shaderModel);	// eg vs_4_0
	wstring fname = WideStringFromString(filename);
	uint flags = GetFlags();
	string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors = null;
	if(SUCCEEDED(D3DCompile(file.data(), file.size(), filename, null, null, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors)))
	{
		HLSLShader *s = new HLSLShader(GetFilename(filename));
		DXB(s->Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc));
		shaders[s->mShaderTypeDesc.type] = s;

		// append to assembly listing file...

		return true;
	}
	if(errors != null)
	{
		Printer::Output("/*\n%s*/\n", errors->GetBufferPointer());
		printf("%s", errors->GetBufferPointer());
		OutputDebugString((LPCSTR)errors->GetBufferPointer());
	}
	else
	{
		Printer::Output("// Unknown D3DCompileFromFile error\n");
	}
	return false;
}

//////////////////////////////////////////////////////////////////////

enum
{
	success,
	err_warnings_issued,
	err_args,
	err_nosource,
	err_noshadermodel,
	err_cantcreateheaderfile,
	err_cantwritetoheaderfile,
	err_compilerproblem,
	err_noshaderspecified,
	err_cantloadsourcefile,
	err_unknownpragma,
	err_malformedpragma
};

//////////////////////////////////////////////////////////////////////

uint current_line;
uint error_count = 0;
uint warning_count = 0;

//////////////////////////////////////////////////////////////////////

void emit_error(char const *format, ...)
{
	va_list v;
	va_start(v, format);
	string e = Format_V(format, v);
	printf("%s(%d): error S0000: %s\n", options[SOURCE_FILE].arg, current_line, e.c_str());
	++error_count;
}

//////////////////////////////////////////////////////////////////////

void emit_warning(char const *format, ...)
{
	va_list v;
	va_start(v, format);
	string e = Format_V(format, v);
	printf("%s(%d): warning T0000: %s\n", options[SOURCE_FILE].arg, current_line, e.c_str());
	++warning_count;
}

//////////////////////////////////////////////////////////////////////

CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);

//////////////////////////////////////////////////////////////////////

enum param_type
{
	bool_param,
	inverse_bool_param,
	enum_param,
	float_param,
	uint8_param,
	int_param
};

//////////////////////////////////////////////////////////////////////

using MAP = std::map<string, uint32>;

struct parameter
{
	char const *name;
	param_type type;
	void *target;						// the default is what's in here to start with (from D3D11_DEFAULT)
	MAP enum_list;	// empty if it's not an enum - this is lame...
};

//////////////////////////////////////////////////////////////////////
// Process key=value entries in #pragma token(key=value)s

std::map<param_type, std::function<void(string const &value, parameter const &param)>> paramHandlerMap =
{
	//////////////////////////////////////////////////////////////////////
	// bool
	{
		bool_param, [] (string const &value, parameter const &param)
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
			TRACE("\t%s = (BOOL)%d\n", param.name, r);
			*((BOOL *)param.target) = r;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// inverse_bool
	{
		inverse_bool_param, [] (string const &value, parameter const &param)
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
			TRACE("\t%s = (INV_BOOL)%d\n", param.name, r);
			*((BOOL *)param.target) = r;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// enum
	{
		enum_param, [] (string const &value, parameter const &param)
		{
			for(auto &e : param.enum_list)
			{
				if(strcmp(value.c_str(), e.first.c_str()) == 0)
				{
					TRACE("\t%s = (ENUM)%s (=%d)\n", param.name, e.first.c_str(), e.second);
					*((DWORD *)param.target) = e.second;
					return;
				}
			}
			emit_error("Unknown enum value %s for %s\n", value.c_str(), param.name);
		}
	},

	//////////////////////////////////////////////////////////////////////
	// float
	{
		float_param, [] (string const &value, parameter const &param)
		{
			char *end;
			float f = strtof(value.c_str(), &end);
			*((float *)param.target) = f;
			TRACE("\t%s = (FLOAT)%f\n", param.name, f);
		}
	},

	//////////////////////////////////////////////////////////////////////
	// uint8
	{
		uint8_param, [] (string const &value, parameter const &param)
		{
			char *end;
			long f = strtol(value.c_str(), &end, 10);
			if(f < 0 || f > 255)
			{
				emit_warning("Value %d out of range (0-255), truncated", f);
			}
			TRACE("\t%s = (UINT8)%d\n", param.name, f);
			*((uint8 *)param.target) = (uint8)f;
		}
	},

	//////////////////////////////////////////////////////////////////////
	// int
	{
		int_param, [] (string const &value, parameter const &param)
		{
			char *end;
			long l = strtol(value.c_str(), &end, 10);
			*((int *)param.target) = l;
			TRACE("\t%s = (INT)%d\n", param.name, l);
		}
	},
};

//////////////////////////////////////////////////////////////////////

MAP depth_write_map =
{
	{ "enabled",			D3D11_DEPTH_WRITE_MASK_ALL		},
	{ "on",					D3D11_DEPTH_WRITE_MASK_ALL		},

	{ "disabled",			D3D11_DEPTH_WRITE_MASK_ZERO		},
	{ "off",				D3D11_DEPTH_WRITE_MASK_ZERO		}
};

//////////////////////////////////////////////////////////////////////

MAP comparison_map =
{
	{ "less",				D3D11_COMPARISON_LESS			},
	{ "equal",				D3D11_COMPARISON_EQUAL			},
	{ "less_equal",			D3D11_COMPARISON_LESS_EQUAL		},
	{ "greater",			D3D11_COMPARISON_GREATER		},
	{ "not_equal",			D3D11_COMPARISON_NOT_EQUAL		},
	{ "greater_equal",		D3D11_COMPARISON_GREATER_EQUAL	},
	{ "always",				D3D11_COMPARISON_ALWAYS			}
};

//////////////////////////////////////////////////////////////////////

MAP stencil_op_map =
{
	{ "keep",				D3D11_STENCIL_OP_KEEP			},
	{ "zero",				D3D11_STENCIL_OP_ZERO			},
	{ "replace",			D3D11_STENCIL_OP_REPLACE		},
	{ "incr_sat",			D3D11_STENCIL_OP_INCR_SAT		},
	{ "decr_sat",			D3D11_STENCIL_OP_DECR_SAT		},
	{ "invert",				D3D11_STENCIL_OP_INVERT			},
	{ "incr",				D3D11_STENCIL_OP_INCR			},
	{ "decr",				D3D11_STENCIL_OP_DECR			}
};

//////////////////////////////////////////////////////////////////////

MAP culling_mode_map =
{
	{ "none",				D3D11_CULL_NONE					},
	{ "front",				D3D11_CULL_FRONT				},
	{ "back",				D3D11_CULL_BACK					}
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
	{ "solid",				D3D11_FILL_SOLID				},
	{ "wireframe",			D3D11_FILL_WIREFRAME			}
};

//////////////////////////////////////////////////////////////////////

MAP blend_op_map =
{
	{ "add",				D3D11_BLEND_OP_ADD			},
	{ "subtract",			D3D11_BLEND_OP_SUBTRACT		},
	{ "rev_subtract",		D3D11_BLEND_OP_REV_SUBTRACT	},
	{ "min",				D3D11_BLEND_OP_MIN			},
	{ "max",				D3D11_BLEND_OP_MAX			}
};

//////////////////////////////////////////////////////////////////////

MAP blend_map =
{
	{ "zero",				D3D11_BLEND_ZERO },
	{ "one",				D3D11_BLEND_ONE },
	{ "src_color",			D3D11_BLEND_SRC_COLOR },
	{ "inv_src_color",		D3D11_BLEND_INV_SRC_COLOR },
	{ "src_alpha",			D3D11_BLEND_SRC_ALPHA },
	{ "inv_src_alpha",		D3D11_BLEND_INV_SRC_ALPHA },
	{ "dest_alpha",			D3D11_BLEND_DEST_ALPHA },
	{ "inv_dest_alpha",		D3D11_BLEND_INV_DEST_ALPHA },
	{ "dest_color",			D3D11_BLEND_DEST_COLOR },
	{ "inv_dest_color",		D3D11_BLEND_INV_DEST_COLOR },
	{ "src_alpha_sat",		D3D11_BLEND_SRC_ALPHA_SAT },
	{ "blend_factor",		D3D11_BLEND_BLEND_FACTOR },
	{ "inv_blend_factor",	D3D11_BLEND_INV_BLEND_FACTOR },
	{ "src1_color",			D3D11_BLEND_SRC1_COLOR },
	{ "inv_src1_color",		D3D11_BLEND_INV_SRC1_COLOR },
	{ "src1_alpha",			D3D11_BLEND_SRC1_ALPHA },
	{ "inv_src1_alpha",		D3D11_BLEND_INV_SRC1_ALPHA }
};

//////////////////////////////////////////////////////////////////////

std::map<string, vector<parameter>> params =
{
	//////////////////////////////////////////////////////////////////////
	// DEPTH_STENCIL_STATE

	{
		"depth", 
		{
			{ "enabled", bool_param, &depthStencilDesc.DepthEnable, {}  },
			{ "disabled", inverse_bool_param, &depthStencilDesc.DepthEnable, {} },
			{ "write", enum_param, &depthStencilDesc.DepthWriteMask, depth_write_map },
			{ "comparison_func", enum_param, &depthStencilDesc.DepthFunc, comparison_map }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"stencil",
		{
			{ "enabled", bool_param, &depthStencilDesc.StencilEnable, {} },
			{ "disabled", inverse_bool_param, &depthStencilDesc.StencilEnable, {} },
			{ "read_mask", uint8_param, &depthStencilDesc.StencilReadMask, {} },
			{ "write_mask", uint8_param, &depthStencilDesc.StencilWriteMask, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"front_stencil",
		{
			{ "fail_op", enum_param, &depthStencilDesc.FrontFace.StencilFailOp, stencil_op_map },
			{ "depth_fail_op", enum_param, &depthStencilDesc.FrontFace.StencilDepthFailOp, stencil_op_map },
			{ "pass_op", enum_param, &depthStencilDesc.FrontFace.StencilPassOp, stencil_op_map },
			{ "comparison_func", enum_param, &depthStencilDesc.FrontFace.StencilFunc, comparison_map }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"back_stencil",
		{
			{ "fail_op", enum_param, &depthStencilDesc.BackFace.StencilFailOp, stencil_op_map },
			{ "depth_fail_op", enum_param, &depthStencilDesc.BackFace.StencilDepthFailOp, stencil_op_map },
			{ "pass_op", enum_param, &depthStencilDesc.BackFace.StencilPassOp, stencil_op_map },
			{ "comparison_func", enum_param, &depthStencilDesc.BackFace.StencilFunc, comparison_map }
		}
	},

	//////////////////////////////////////////////////////////////////////
	// RASTERIZER_STATE

	{
		"culling",
		{
			{ "mode", enum_param, &rasterizerDesc.CullMode, culling_mode_map },
			{ "order", enum_param, &rasterizerDesc.FrontCounterClockwise, culling_order_map }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"fill",
		{
			{ "mode", enum_param, &rasterizerDesc.FillMode, fill_mode_map }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"depth_bias",
		{
			{ "bias", int_param, &rasterizerDesc.DepthBias, {} },
			{ "clamp", float_param, &rasterizerDesc.DepthBiasClamp, {} },
			{ "slope_scaled_bias", float_param, &rasterizerDesc.SlopeScaledDepthBias, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"depth_clip",
		{
			{ "enabled", bool_param, &rasterizerDesc.DepthClipEnable, {} },
			{ "disabled", inverse_bool_param, &rasterizerDesc.DepthClipEnable, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"scissor",
		{
			{ "enabled", bool_param, &rasterizerDesc.ScissorEnable, {} },
			{ "disabled", inverse_bool_param, &rasterizerDesc.ScissorEnable, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"multisample",
		{
			{ "enabled", bool_param, &rasterizerDesc.MultisampleEnable, {} },
			{ "disabled", inverse_bool_param, &rasterizerDesc.MultisampleEnable, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////

	{
		"anti_aliased_line",
		{
			{ "enabled", bool_param, &rasterizerDesc.AntialiasedLineEnable, {} },
			{ "disabled", inverse_bool_param, &rasterizerDesc.AntialiasedLineEnable, {} }
		}
	},

	//////////////////////////////////////////////////////////////////////
	// BLEND_STATE
	{
		"blend_0",
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
};

//////////////////////////////////////////////////////////////////////

uint ScanMaterialOptions(Resource &file, string &result)
{
	std::regex rgx(R"(^#pragma\s+(.+)\((.*)\))");

	using keyvals = std::map<string, string>;
	using paramList = std::map<string, keyvals>;

	paramList parameters;

	vector<string> lines;
	std::istringstream str((char *)file.Data());
	std::ostringstream out;
	string line;
	current_line = 1;
	while(std::getline(str, line))
	{
		std::cmatch m;
		if(regex_search(line.c_str(), m, rgx))
		{
			string token = m[1].str();
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
					emit_error("Malformed Pragma");
					return err_malformedpragma;
				}
			}
			auto f = parameters.find(token);
			if(f != parameters.end())
			{
				// warn if a value already set is being overwritten
				for(auto p = kvalues.begin(); p != kvalues.end(); ++p)
				{
					f->second[p->first] = p->second;
				}
			}
			else
			{
				string paramName = token;
				keyvals &keyVals = kvalues;

				TRACE("%s\n", paramName.c_str());

				auto p = params.find(paramName);
				if(p == params.end())
				{
					emit_warning("unknown pragma: %s", paramName.c_str());
					continue;
				}

				line = string("// ") + line;

				// Now that we have a pragma we know about, we need to nobble it so
				// the shader compiler doesn't emit a warning about an unknown pragma

				for(auto &keyval : keyVals)
				{
					string keyname = keyval.first;
					string keyValue = keyval.second;

					//TRACE("Key [%s,%s]\n", keyname.c_str(), keyValue.c_str());

					bool valid = false;

					for(auto &option : p->second)
					{
						//TRACE("Is it option %s?\n", option.name);
						// q is a reference to a parameter
						// is keyName == q.name
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
								//TRACE("Yep\n");
								fnc->second(keyValue, option);
							}
						}
					}
				}
			}
		}
		out.write(line.c_str(), line.size());
		out.write("\n", 1);
		++current_line;
	}
	result = out.str();
	return success;
}

//////////////////////////////////////////////////////////////////////

template <typename T> void OutputStruct(string const &name, T const &value, char const *typeName, char const *varName)
{
	using namespace Printer;

	OutputLine("// %s", typeName);
	OutputLine("uint32 WEAKSYM %s_%s[] = ", name.c_str(), varName);
	OutputIndent("{");
	Indent();
	OutputBinary(&value, sizeof(T));
	UnIndent("};");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void OutputMaterial(string const &name)
{
	OutputStruct(name, depthStencilDesc, "D3D11_DEPTH_STENCIL_DESC", "DepthStencilDesc");
	OutputStruct(name, rasterizerDesc, "D3D11_RASTERIZER_DESC", "RasterizerDesc");
	OutputStruct(name, blendDesc, "D3D11_BLEND_DESC", "BlendDesc");
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	if(!ParseArgs(argc, argv, options))
	{
		emit_error("Usage");
		return err_args;
	}

	if(!options[SOURCE_FILE])
	{
		emit_error("No source file specified");
		PrintUsage();
		return err_nosource;
	}

	if(options[EMBED_BYTECODE])
	{
		// Bytecode for all shaders goes into a file called Phong.shader.bytecode
		// Header is a set of N dwords which specify length of each shader (0 means no shader in the slot)
		// DWORD magic = 'SHDR'
		// DWORD lengths[NumShaderTypes]
		// Followed by the bytecodes
	}

	if(!options[SHADER_MODEL_VERSION])
	{
		emit_error("Shader model not specified");
		PrintUsage();
		return err_noshadermodel;
	}

	Handle headerFile;
	if(options[HEADER_FILE])
	{
		char const *name = options[HEADER_FILE].arg;
		headerFile = CreateFile(name, GENERIC_WRITE, 0, null, CREATE_ALWAYS, 0, null);
	}
	else
	{
		headerFile = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	if(!headerFile.IsValid())
	{
		emit_error("Error opening header file");
		return err_cantcreateheaderfile;
	}

	Printer::SetHeaderFile(headerFile);

	string fileName = GetFilename(options[SOURCE_FILE].arg);

	current_line = 1;

	FileResource sourceFile(options[SOURCE_FILE].arg);
	if(!sourceFile.IsValid())
	{
		emit_error("Error loading %s", options[SOURCE_FILE].arg);
		return err_cantloadsourcefile;
	}

	string modified_source;
	ScanMaterialOptions(sourceFile, modified_source);

	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(modified_source, options[SOURCE_FILE].arg, options[a.arg].arg, options[SHADER_MODEL_VERSION].arg, a.type))
			{
				return err_compilerproblem;
			}
			++shadersCompiled;
		}
	}

	if(shadersCompiled == 0)
	{
		emit_error("No shader types specified");
		return err_noshaderspecified;
	}

	char const *namespaceIn = options[NAMESPACE] ? options[NAMESPACE].arg : null;

	HLSLShader::OutputHeader(fileName.c_str(), namespaceIn);

	string name = GetFilename(options[SOURCE_FILE].arg);

	OutputMaterial(name);

	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputBlob();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputConstBufferNamesAndOffsets();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputSamplerNames();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputResourceNames();
	}
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputInputElements();
	}

	using namespace Printer;

	OutputCommentLine("Shader struct");
	OutputLine("struct %s : ShaderState", name.c_str());
	OutputIndent("{");
	OutputLine();
	Indent();
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputHeaderFile();
	}

	OutputCommentLine("Members");

	// now members
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		(*i).second->OutputMemberVariable();
	}

	// Material bits
	OutputLine();
	OutputLine("DXPtr<ID3D11DepthStencilState> depthStencilState;");
	OutputLine("DXPtr<ID3D11RasterizerState> rasterizerState;");
	OutputLine("DXPtr<ID3D11BlendState> blendState;");

	OutputLine();
	OutputCommentLine("Constructor");
	OutputLine("%s()", name.c_str());
	Indent("{");
	OutputLine();
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		char const *name = ShaderTypeDescs[i].name;
		auto f = shaders.find((ShaderType)i);
		if(f == shaders.end())
		{
			OutputLine("Shaders[%s] = null;", name);
		}
		else
		{
			OutputLine("Shaders[%s] = &%s;", name, ToLower(ShaderTypeDescs[i].refName).c_str());;
		}
	}

	OutputLine();
	OutputLine("DX::Device->CreateDepthStencilState((D3D11_DEPTH_STENCIL_DESC const *)%s_DepthStencilDesc, &depthStencilState);", name.c_str());
	OutputLine("DX::Device->CreateRasterizerState((D3D11_RASTERIZER_DESC const *)%s_RasterizerDesc, &rasterizerState);", name.c_str());
	OutputLine("DX::Device->CreateBlendState((D3D11_BLEND_DESC const *)%s_BlendDesc, &blendState);", name.c_str());

	UnIndent("}");

	OutputLine();
	OutputCommentLine("Activate");
	OutputLine("void Activate(ID3D11DeviceContext *context)");
	Indent("{");
	OutputLine();

	OutputLine("context->OMSetDepthStencilState(depthStencilState, 0);");
	OutputLine("context->OMSetBlendState(blendState, null, 0xffffffff);");
	OutputLine("context->RSSetState(rasterizerState);");
	OutputLine();

	// Activate
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		auto f = shaders.find((ShaderType)i);
		if(f == shaders.end())
		{
			OutputLine("context->%sSetShader(null, null, 0);", ShaderTypeDescs[i].refName);
		}
		else
		{
			OutputLine("%s.Activate(context);", ToLower(ShaderTypeDescs[i].refName).c_str());
		}
	}
	UnIndent("}");
	OutputLine();
	OutputLine("void Activate_V(ID3D11DeviceContext *context) override");
	Indent("{");
	OutputLine();
	OutputLine("Activate(context);");
	UnIndent("}");
	UnIndent("};");

	HLSLShader::OutputFooter(fileName.c_str(), namespaceIn);

	return (options[ERROR_ON_WARNING] && warning_count > 0) ? err_warnings_issued : success;
}
