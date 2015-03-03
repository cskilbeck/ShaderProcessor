//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

using namespace DirectX;
using option::Option;

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
	uint flags = GetCompilerOptionFlags();
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
		printf("%s", errors->GetBufferPointer());
	}
	else
	{
		emit_error("unknown D3DCompiler error");
	}
	return false;
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

uint SaveSOBFile(string const &binFile)
{
	uint32 offsets[NumShaderTypes] = { 0 };
	uint32 sizes[NumShaderTypes] = { 0 };

	// get the sizes & offsets
	for(auto &i : shaders)
	{
		auto s = *i.second;
		uint32 size = (uint32)s.mSize;
		uint type = s.mShaderType;
		sizes[type] = size;
	}

	uint currentOffset = sizeof(offsets);
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		offsets[i] = currentOffset;
		currentOffset += sizes[i];
	}

	// create the file
	File f;
	if(!f.Create(binFile.c_str()))
	{
		emit_error("Can't create %s (%08x)", binFile.c_str(), f.error);
	}
	else
	{
		// write the offsets
		if(f.Write(sizeof(offsets), offsets) == 0)
		{
			emit_error("Can't write to %s (%08x)", binFile.c_str(), f.error);
		}
		else
		{
			// write the blobs
			for(auto &i : shaders)
			{
				auto s = *i.second;
				if(f.Write((uint32)s.mSize, s.mBlob) == 0)
				{
					emit_error("Can't write to %s (%08x)", binFile.c_str(), f.error);
					break;
				}
			}
		}
	}
	return success;
}

//////////////////////////////////////////////////////////////////////

using ShaderFunc = void(HLSLShader::*)();

template<ShaderFunc f> void Loop(std::map<ShaderType, HLSLShader *> &shaders)
{
	for(auto i = shaders.begin(); i != shaders.end(); ++i)
	{
		((*i).second->*f)();
	}
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

	Printer::SetHeaderFile(headerFile);

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
		outpath = AppendBackslash(options[DATA_ROOT].arg);
	}
	if(options[DATA_FOLDER])
	{
		dataFolder = AppendBackslash(options[DATA_FOLDER].arg);
		outpath.append(dataFolder);
		relativeBinFile = Format("%s%s.sob", dataFolder.c_str(), name.c_str());
		binFile = Format("%s%s.sob", outpath.c_str(), name.c_str());
	}

	using namespace Printer;

	if(!options[EMBED_BYTECODE])
	{
		uint err = SaveSOBFile(binFile);

		if(err != success)
		{
			return err;
		}
	}
	else
	{
		Loop<&HLSLShader::OutputBlob>(shaders);
	}

	Loop<&HLSLShader::OutputConstBufferNamesAndOffsets>(shaders);
	Loop<&HLSLShader::OutputSamplerNames>(shaders);
	Loop<&HLSLShader::OutputResourceNames>(shaders);
	Loop<&HLSLShader::OutputInputElements>(shaders);

	// We need to know that the samplers, resources and constbuffers are sorted by their binding #
	// they seem to be but it's not stated in the docs that it is guaranteed to be so
	// therefore, we should sort the samplers, resources and constbuffers by their binding #
	// but I can't be arsed and it's probably not necessary

	OutputCommentLine("Binding runs");
	OutputIndent("extern Shader::BindingState WEAKSYM %s_Bindings[] = {", name.c_str());
	Indent();
	char const *sep = "";
	for(int i = 0; i < NumShaderTypes; ++i)
	{
		Output(sep);
		OutputLine();
		OutputIndent();
		Output("{");
		if(shader_array[i] != null)
		{
			shader_array[i]->OutputBindingRuns();
		}
		Output("}");
		sep = ",";
	}
	UnIndent();
	OutputLine();
	OutputLine("};");
	OutputLine();

	OutputCommentLine("Shader struct");
	OutputLine("struct %s : ShaderState", name.c_str());
	OutputIndent("{");
	OutputLine();
	Indent();

	Loop<&HLSLShader::OutputHeaderFile>(shaders);

	OutputCommentLine("Members");

	Loop<&HLSLShader::OutputMemberVariable>(shaders);

	OutputLine();

	string shaderCtors = Format("%s_BlendDesc, ", name.c_str());
	shaderCtors += Format("%s_DepthStencilDesc, ", name.c_str());
	shaderCtors += Format("%s_RasterizerDesc", name.c_str());

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
		OutputLine("%s.%s(%s%s);", ToLower(shader.RefName()).c_str(), func, params.c_str(), shader.VSTag().c_str());
	}
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		OutputLine("Shaders[%s] = %s;", ShaderTypeDescs[i].name, (shader_array[i] == null) ? "null" : Format("&%s", ToLower(ShaderTypeDescs[i].refName).c_str()).c_str());
	}
	UnIndent();
	OutputLine("}");
	OutputLine();

	// Activate
	OutputCommentLine("Activate");
	OutputLine("void Activate(ID3D11DeviceContext *context)");
	Indent("{");
	OutputLine();
	OutputLine("ShaderState::SetState(context);");

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
