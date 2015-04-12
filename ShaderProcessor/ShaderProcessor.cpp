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
	//string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors = null;
	if(SUCCEEDED(D3DCompile((void *)file.data(), file.size(), filename, null, null, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors)))
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

int CompileShaders(string &file)
{
	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(file, options[SOURCE_FILE].arg, options[a.arg].arg, options[SHADER_MODEL_VERSION].arg, a.type))
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

int CreateSOB(MemoryFile &memFile)
{
	uint32 offsets[NumShaderTypes] = { 0 };
	uint32 sizes[NumShaderTypes] = { 0 };

	uint64 totalSize = sizeof(offsets);

	// get the sizes & offsets
	for(auto &i : shaders)
	{
		auto s = *i.second;
		uint32 size = (uint32)s.mSize;
		uint type = s.mShaderType;
		sizes[type] = size;
		totalSize += size;
	}

	uint currentOffset = sizeof(offsets);
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		offsets[i] = currentOffset;
		currentOffset += sizes[i];
	}

	memFile.Reset(totalSize);

	// write the offsets
	uint64 wrote;
	int e = memFile.Write(offsets, sizeof(offsets), &wrote);
	if(e != S_OK)
	{
		emit_error("Can't write to SOB blob (%08x)", e);
		return e;
	}
	if(wrote != sizeof(offsets))
	{
		emit_error("Error writing to SOB blob");
		return E_FAIL;
	}
	// write the blobs
	for(auto &i : shaders)
	{
		auto s = *i.second;
		int e = memFile.Write(s.mBlob, s.mSize, &wrote);
		if(e != S_OK)
		{
			emit_error("Can't write to %s (%08x)", binFile.c_str(), e);
			return e;
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

int SaveSOBFile(string const &binFile, MemoryFile &memFile)
{
	uint64 size;
	DXR(memFile.GetSize(size));
	DXR(SaveFile(binFile.c_str(), memFile.ptr, (uint32)size));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

int EmitSOBFile(MemoryFile &memFile)
{
	using namespace Printer;
	OutputCommentLine("SOB");
	OutputLine("extern uint32 const WEAKSYM %s_SOB[] = ", shaderName.c_str());
	OutputIndent("{");
	Indent();
	OutputBinary(memFile.ptr, (uint32)memFile.size);
	UnIndent();
	OutputLine("};");
	OutputLine();
	return S_OK;
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

int OpenHeaderFile(DiskFile &headerFile)
{
	if(options[HEADER_FILE])
	{
		char const *name = options[HEADER_FILE].arg;
		if(headerFile.Create(name, DiskFile::Overwrite) != S_OK)
		{
			return err_cantcreateheaderfile;
		}
	}
	else
	{
		headerFile.Acquire(GetStdHandle(STD_OUTPUT_HANDLE));
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

	if(shader_array[ShaderType::Vertex] != null)
	{
		shader_array[ShaderType::Vertex]->OutputVertexBufferBindingRuns();
	}
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

	OutputCommentLine("Constructor/Destructor/Load/Release");
	OutputLine("%s(): ShaderState()", shaderName.c_str());
	Indent("{");
	OutputLine();
	UnIndent("}");
	OutputLine();

	OutputLine("~%s()", shaderName.c_str());
	Indent("{");
	OutputLine();
//	OutputLine("Release();");
	UnIndent("}");
	OutputLine();

	OutputLine("HRESULT Create()");
	Indent("{");
	OutputLine();
	OutputLine("TRACE(\"%s::Create()\\n\");", shaderName.c_str());
	OutputLine("DXR(ShaderState::Create(%s));", shaderCtors.c_str());
	if(!options[EMBED_BYTECODE])
	{
		OutputLine("FileResource sob;");
		OutputLine("DXR(AssetManager::LoadFile(TEXT(\"%s\"), sob));", relativeBinFile.c_str());
	}
	else
	{
		OutputLine("MemoryResource sob(%s_SOB, sizeof(%s_SOB));", shaderName.c_str(), shaderName.c_str());
	}

	for(auto &s : shaders)
	{
		auto &shader = *s.second;
		OutputLine("%s.Create(sob);", ToLower(shader.RefName()).c_str());
		OutputLine("%s.SetName(\"%s\");", ToLower(shader.RefName()).c_str(), shaderName.c_str());
	}
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		OutputLine("Shaders[%s] = %s;", ShaderTypeDescs[i].name, (shader_array[i] == null) ? "null" : Format("&%s", ToLower(ShaderTypeDescs[i].refName).c_str()).c_str());
	}
	OutputLine("return S_OK;");
	UnIndent();
	OutputLine("}");
	OutputLine();

	OutputLine("void Release()");
	Indent("{");
	OutputLine();
	OutputLine("TRACE(\"%s::Release();\\n\");", shaderName.c_str());
	OutputLine("ShaderState::Release();");
	for(uint i = 0; i < NumShaderTypes; ++i)
	{
		if(shader_array[i] != null)
		{
			OutputLine("%s.Release();", ToLower(ShaderTypeDescs[i].refName).c_str());
		}
	}
	UnIndent("}");
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
	switch(e)
	{
	case err_okbutquit:
		return success;
	case success:
		break;
	default:
		return e;
	}

	string file;

	// Scan for render material options
	e = ScanMaterialOptions(options[SOURCE_FILE].arg, file);
	if(e != success)
	{
		return e;
	}

	// compile the shaders
	e = CompileShaders(file);
	if(e != success)
	{
		return e;
	}

	// work out where the output is going to go
	DiskFile headerFile;
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
	}
	else
	{
		dataFolder = "";
	}
	relativeBinFile = ReplaceAll(Format(TEXT("%s%s.sob"), dataFolder.c_str(), shaderName.c_str()), tstring("\\"), tstring("\\\\"));
	binFile = Format("%s%s.sob", outpath.c_str(), shaderName.c_str());

	// Right, the output

	char const *namespaceIn = options[NAMESPACE] ? options[NAMESPACE].arg : null;
	
	using namespace Printer;
	SetHeaderFile(headerFile.h);
	OutputHeader(shaderName.c_str(), namespaceIn);
	OutputMaterial();

	// Create a SOB file in memory
	// Output it to the disk or the header file
	// Use it as-is (either loaded or from the header) the same way
	MemoryFile memFile;
	CreateSOB(memFile);

	if(!options[EMBED_BYTECODE])
	{
		uint err = SaveSOBFile(binFile, memFile);
		if(err != success)
		{
			return err;
		}
	}
	else
	{
		EmitSOBFile(memFile);
//		Loop<&HLSLShader::OutputBlob>(shaders);
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
