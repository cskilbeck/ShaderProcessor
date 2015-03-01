//////////////////////////////////////////////////////////////////////

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
HLSLShader *shader_array[NumShaderTypes] = { 0 };

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
		shader_array[s->mShaderTypeDesc.type] = s;
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
	err_malformedpragma,
	err_unknownkey
};

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

std::map<param_type, char const *> param_names =
{
	{ bool_param, "bool" },
	{ inverse_bool_param, "bool" },
	{ enum_param, "enum" },
	{ float_param, "float" },
	{ uint8_param, "byte" },
	{ int_param, "int" }
};

std::map<param_type, char const *> param_print_strings =
{
	{ bool_param, "%d" },
	{ inverse_bool_param, "%d" },
	{ float_param, "%f" },
	{ uint8_param, "%02x" },
	{ int_param, "%d" }
};

char const *param_type_name(param_type t)
{
	auto f = param_names.find(t);
	return (f == param_names.end()) ? "??" : f->second;
}

char const *param_printf_string(param_type t)
{
	auto f = param_print_strings.find(t);
	return (f == param_print_strings.end()) ? "" : f->second;
}

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
			TRACE("\t%s = (BOOL)%d\n", param.name, r);
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
			TRACE("\t%s = (INV_BOOL)%d\n", param.name, r);
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
					TRACE("\t%s = (ENUM)%s (=%d)\n", param.name, e.first.c_str(), e.second);
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
			TRACE("\t%s = (FLOAT)%f\n", param.name, f);
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
			TRACE("\t%s = (UINT8)%d\n", param.name, f);
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
					printf("%s (default = 0x%s)\n", param_type_name(p.type),Format(param_printf_string(p.type), *((uint8 *)p.target)).c_str());
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
	Error::current_line = 1;
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

					//TRACE("Key [%s,%s]\n", keyname.c_str(), keyValue.c_str());

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
								//TRACE("Yep\n");
								for(auto index : indices)
								{
									TRACE("Index %d:", index);
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
		out.write(line.c_str(), line.size());
		out.write("\n", 1);
		++Error::current_line;
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

	if(options[PRAGMA_OPTIONS])
	{
		OutputPragmaDocs();
		return success;
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

	Error::current_line = 1;

	FileResource sourceFile(options[SOURCE_FILE].arg);
	if(!sourceFile.IsValid())
	{
		emit_error("Error loading %s", options[SOURCE_FILE].arg);
		return err_cantloadsourcefile;
	}

	string modified_source;
	int e = ScanMaterialOptions(sourceFile, modified_source);
	if(e != success)
	{
		return e;
	}

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

	// work out filename
	string outpath;
	string dataFolder;
	string relativeBinFile;
	string binFile;
	if(options[DATA_ROOT])
	{
		outpath = options[DATA_ROOT].arg;
		if(outpath.back() != '\\')
		{
			outpath.append("\\");
		}
	}
	if(options[DATA_FOLDER])
	{
		dataFolder = options[DATA_FOLDER].arg;
		if(dataFolder.back() != '\\')
		{
			dataFolder.append("\\");
		}
		outpath.append(dataFolder);
		relativeBinFile = Format("%s%s.sob", dataFolder.c_str(), name.c_str());
		binFile = Format("%s%s.sob", outpath.c_str(), name.c_str());
	}

	// save the .bin file if embed_bytecode isn't specified
	if(!options[EMBED_BYTECODE])
	{
		uint32 offsets[NumShaderTypes] = { 0 };
		uint32 sizes[NumShaderTypes] = { 0 };

		// get the sizes
		for(auto &i : shaders)
		{
			auto s = *i.second;
			sizes[s.mShaderType] = (uint32)s.mSize;
		}

		// work out the offsets
		uint currentOffset = sizeof(offsets);
		for(uint i = 0; i < NumShaderTypes; ++i)
		{
			offsets[i] = currentOffset;
			currentOffset += sizes[i];
		}

		// create the file
		File f;
		f.Create(binFile.c_str());

		// write the offsets
		f.Write(sizeof(offsets), offsets);

		// write the blobs
		for(auto &i : shaders)
		{
			auto s = *i.second;
			f.Write((uint32)s.mSize, s.mBlob);
		}
	}

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

	string shaderCtors;

	shaderCtors = Format("%s_BlendDesc, ", name.c_str());
	shaderCtors += Format("%s_DepthStencilDesc, ", name.c_str());
	shaderCtors += Format("%s_RasterizerDesc", name.c_str());

	// vs, ps etc else null

	OutputCommentLine("Constructor");
	OutputLine("%s(): ShaderState(%s)", name.c_str(), shaderCtors.c_str());
	Indent("{");
	OutputLine();
	char const *func;
	if(!options[EMBED_BYTECODE])
	{
		OutputLine("FileResource f(TEXT(\"%s\"));", ReplaceAll(relativeBinFile, string("\\"), string("\\\\")).c_str());
		func = "Load";
	}
	else
	{
		func = "Create";
	}

	for(auto &s : shaders)
	{
		auto &shader = *s.second;
		string params;
		if(options[EMBED_BYTECODE])
		{
			params = Format("%s_%s_Data, %d", name.c_str(), shader.RefName().c_str(), shader.mSize);
		}
		else
		{
			params = "f";
		}
		// ps.Load(f);
		// vs.Create(Shader_VS_Data, 1152, Shader_InputElements, _countof(Shader_InputElements));
		OutputLine("%s.%s(%s%s);", ToLower(shader.RefName()).c_str(), func, params.c_str(), shader.VSTag().c_str());
	}
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		OutputLine("Shaders[%s] = %s;", ShaderTypeDescs[i].name, (shader_array[i] == null) ? "null" : Format("&%s", ToLower(ShaderTypeDescs[i].refName).c_str()).c_str());
	}
	UnIndent();
	OutputLine("}");

	OutputLine();
	OutputCommentLine("Activate");
	OutputLine("void Activate(ID3D11DeviceContext *context)");
	Indent("{");
	OutputLine();
	OutputLine("ShaderState::SetState(context);");

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

	return (options[ERROR_ON_WARNING] && Error::warning_count > 0) ? err_warnings_issued : success;
}
