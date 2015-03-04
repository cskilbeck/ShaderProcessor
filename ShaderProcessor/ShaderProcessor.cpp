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

string shaderName;
string outpath;
string dataFolder;
string relativeBinFile;
string binFile;

//////////////////////////////////////////////////////////////////////

bool CompileFile(FileResource &file, char const *filename, char const *mainFunction, char const *shaderModel, ShaderType type)
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
	//string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors = null;
	if(SUCCEEDED(D3DCompile(file.Data(), file.Size(), filename, null, null, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors)))
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

int CompileShaders()
{
	FileResource sourceFile(options[SOURCE_FILE].arg);
	if(!sourceFile.IsValid())
	{
		emit_error("Error loading %s", options[SOURCE_FILE].arg);
		return err_cantloadsourcefile;
	}

	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(sourceFile, options[SOURCE_FILE].arg, options[a.arg].arg, options[SHADER_MODEL_VERSION].arg, a.type))
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

void OutputMaterial()
{
	OutputStruct(shaderName, depthStencilDesc, "D3D11_DEPTH_STENCIL_DESC", "DepthStencilDesc");
	OutputStruct(shaderName, rasterizerDesc, "D3D11_RASTERIZER_DESC", "RasterizerDesc");
	OutputStruct(shaderName, blendDesc, "D3D11_BLEND_DESC", "BlendDesc");
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
		uint32 wrote;
		if(!f.Write(sizeof(offsets), offsets, wrote))
		{
			emit_error("Can't write to %s (%08x)", binFile.c_str(), f.error);
		}
		else
		{
			// write the blobs
			for(auto &i : shaders)
			{
				auto s = *i.second;
				if(!f.Write((uint32)s.mSize, s.mBlob, wrote))
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

void OutputHeader(char const *filename, char const *namespace_) // static
{
	using namespace Printer;
	OutputCommentLine("%s.h - auto generated file, do not edit", filename);
	OutputLine("#pragma once");
	OutputLine("#pragma pack(push, 4)");
	OutputLine();
	if(namespace_ != null)
	{
		OutputLine("namespace %s", namespace_);
		Indent("{");
		OutputLine();
	}
	OutputLine("using namespace DX;");
	OutputLine();
}

//////////////////////////////////////////////////////////////////////

void OutputFooter(char const *filename, char const *namespace_) // static
{
	using namespace Printer;
	if(namespace_ != null)
	{
		UnIndent("}");
		OutputLine();
	}
	OutputLine("#pragma pack(pop)");
}

//////////////////////////////////////////////////////////////////////

int OpenHeaderFile(File &headerFile)
{
	if(options[HEADER_FILE])
	{
		char const *name = options[HEADER_FILE].arg;
		headerFile.Create(name);
	}
	else
	{
		headerFile.Acquire(GetStdHandle(STD_OUTPUT_HANDLE));
	}

	if(!headerFile.IsOpen())
	{
		emit_error("Error opening header file");
		return err_cantcreateheaderfile;
	}
	return success;
}

//////////////////////////////////////////////////////////////////////

void OutputBindingRuns()
{
	// We need to know that the samplers, resources and constbuffers are sorted by their binding #
	// they seem to be but it's not stated in the docs that it is guaranteed to be so
	// therefore, we should sort the samplers, resources and constbuffers by their binding #
	// but I can't be arsed and it's probably not necessary

	using namespace Printer;
	OutputCommentLine("Binding runs");
	OutputIndent("extern Shader::BindingState WEAKSYM %s_Bindings[] = {", shaderName.c_str());
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

void OutputShaderStruct()
{
	using namespace Printer;
	OutputCommentLine("Shader struct");
	OutputLine("struct %s : ShaderState", shaderName.c_str());
	OutputIndent("{");
	OutputLine();
	Indent();
	Loop<&HLSLShader::OutputHeaderFile>(shaders);

	OutputCommentLine("Members");
	Loop<&HLSLShader::OutputMemberVariable>(shaders);
	OutputLine();

	string shaderCtors = Format("%s_BlendDesc, ", shaderName.c_str());
	shaderCtors += Format("%s_DepthStencilDesc, ", shaderName.c_str());
	shaderCtors += Format("%s_RasterizerDesc", shaderName.c_str());

	OutputCommentLine("Constructor");
	OutputLine("%s(): ShaderState(%s)", shaderName.c_str(), shaderCtors.c_str());
	Indent("{");
	OutputLine();
	char const *func;
	if(!options[EMBED_BYTECODE])
	{
		OutputLine("FileResource f(TEXT(\"%s\"));", relativeBinFile.c_str());
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
			params = Format("%s_%s_Data, %d", shaderName.c_str(), shader.RefName().c_str(), shader.mSize);
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
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	// parse and validate the arguments
	int e = CheckArgs(argc, argv);
	if(e != success)
	{
		return e;
	}

	// Scan for render material options
	e = ScanMaterialOptions(options[SOURCE_FILE].arg);
	if(e != success)
	{
		return e;
	}

	// compile the shaders
	e = CompileShaders();
	if(e != success)
	{
		return e;
	}

	// work out where the output is going to go
	File headerFile;
	e = OpenHeaderFile(headerFile);
	if(e != success)
	{
		return e;
	}

	// work out filenames
	shaderName = GetFilename(options[SOURCE_FILE].arg);
	if(options[DATA_ROOT])
	{
		outpath = AppendBackslash(options[DATA_ROOT].arg);
	}
	if(options[DATA_FOLDER])
	{
		dataFolder = AppendBackslash(options[DATA_FOLDER].arg);
		outpath.append(dataFolder);
		relativeBinFile = ReplaceAll(Format(TEXT("%s%s.sob"), dataFolder.c_str(), shaderName.c_str()), tstring("\\"), tstring("\\\\"));
		binFile = Format("%s%s.sob", outpath.c_str(), shaderName.c_str());
	}

	// Right, the output

	char const *namespaceIn = options[NAMESPACE] ? options[NAMESPACE].arg : null;
	
	using namespace Printer;
	SetHeaderFile(headerFile);
	OutputHeader(shaderName.c_str(), namespaceIn);
	OutputMaterial();

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

	OutputBindingRuns();

	OutputShaderStruct();

	OutputFooter(shaderName.c_str(), namespaceIn);

	return (options[ERROR_ON_WARNING] && Error::warning_count > 0) ? err_warnings_issued : success;
}
