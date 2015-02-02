//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "output.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

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
	{ VERTEXMAIN, ShaderType::Vertex },
	{ PIXELMAIN, ShaderType::Pixel }
};

//////////////////////////////////////////////////////////////////////

bool CompileFile(char const *filename, char const *mainFunction, char const *shaderModel, ShaderType type)
{
	ShaderTypeDesc const *desc = ShaderTypeDescFromType(type);
	if(desc == null)
	{
		fprintf(stderr, "Unknown shader type: %d\n", type);
		return false;
	}
	string shader = Format("%s_%s", desc->refName, shaderModel);	// eg vs_4_0
	wstring fname = WideStringFromString(filename);
	uint flags = 0;
	string output = SetExtension(filename, TEXT(".cso"));
	ID3DBlob *compiledShader;
	ID3DBlob *errors;
	if(D3DCompileFromFile(fname.c_str(), null, D3D_COMPILE_STANDARD_FILE_INCLUDE, mainFunction, shader.c_str(), flags, 0, &compiledShader, &errors) == S_OK)
	{
		HLSLShader s(GetFilename(filename));
		s.Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), *desc);
		return true;
	}
	fprintf(stderr, "%s\n", errors->GetBufferPointer());
	return false;
}

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	Ptr<vs_shader> v;
	Ptr<ps_shader> p;
	Ptr<Texture> t;
	Ptr<Sampler> s;
	Ptr<vs_shader::VertexBuffer> vb;
	DXPtr<ID3D11RasterizerState> rasterizerState;
	DXPtr<ID3D11DepthStencilState>	depthStencilState;
	DXPtr<ID3D11BlendState>	blendState;

	MyDXWindow()
		: DXWindow(640, 480)
	{
	}

	void OnKeyDown(int key, uintptr flags) override
	{
		if(key == 27)
		{
			Close();
		}
	}

	bool OnCreate() override
	{
		if(!DXWindow::OnCreate())
		{
			return false;
		}
		v.reset(new vs_shader());
		p.reset(new ps_shader());
		t.reset(new Texture(TEXT("temp.png")));
		s.reset(new Sampler());
		vb.reset(new vs_shader::VertexBuffer(4));

		vs_shader::VertexBuffer &b = *vb;
		b[0].Color = Color::White;	b[0].Position = Float2(0.0f, 1);	b[0].TexCoord = Float2(0, 0);
		b[1].Color = Color::White;	b[1].Position = Float2(0.0f, 0);	b[1].TexCoord = Float2(0, 1);
		b[2].Color = Color::White;	b[2].Position = Float2(0.5f, 1);	b[2].TexCoord = Float2(1, 0);
		b[3].Color = Color::White;	b[3].Position = Float2(0.5f, 0);	b[3].TexCoord = Float2(1, 1);
		b.Commit(Context());

		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		DXB(Device()->CreateRasterizerState(&rasterizerDesc, &rasterizerState));

		CD3D11_DEPTH_STENCIL_DESC depthstencilDesc(D3D11_DEFAULT);
		depthstencilDesc.DepthEnable = false;
		depthstencilDesc.StencilEnable = false;
		DXB(Device()->CreateDepthStencilState(&depthstencilDesc, &depthStencilState));

		CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		DXB(Device()->CreateBlendState(&blendDesc, &blendState));

		v->VertConstants.ProjectionMatrix.m[0][0] = 0.3f;
		v->VertConstants.ProjectionMatrix.m[1][1] = 0.3f;
		v->VertConstants.Commit(Context());

		p->tex1Sampler = s.get();
		p->picTexture = t.get();

		return true;
	}

	void OnDraw() override
	{
		Clear(Color(32, 64, 128));

		p->ColourStuff.tint = Float4(1.0f, 1.0f, 1.0f, sinf(mFrame * 0.25f) * 0.25f + 0.75f);
		p->ColourStuff.Commit(Context());

		v->Activate(Context());
		p->Activate(Context());
		vb->Activate(Context());

		Context()->RSSetState(rasterizerState);
		Context()->OMSetBlendState(blendState, null, 0xffffffff);
		Context()->OMSetDepthStencilState(depthStencilState, 0);

		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Context()->Draw(4, 0);
	}

	void OnDestroy() override
	{
		v.reset();
		p.reset();
		t.reset();
		s.reset();
		vb.reset();
		rasterizerState.Release();
		depthStencilState.Release();
		blendState.Release();
	}
};

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	MyDXWindow w;
	w.Open();
	w.Show();

	while(w.Update())
	{
	}

	vector<Option> options;
	if(!ParseArgs(argc, argv, options))
	{
		return 1;
	}

	if(!options[SOURCE])
	{
		fprintf(stderr, "No source file specified, exiting\n\n");
		PrintUsage();
		return 1;
	}

	if(!options[SMVERSION])
	{
		fprintf(stderr, "Shader model not specified, exiting\n");
		PrintUsage();
		return 1;
	}

	Handle headerFile;
	if(options[HEADER])
	{
		char const *name = options[HEADER].arg;
		headerFile = CreateFile(name, GENERIC_WRITE, 0, null, CREATE_ALWAYS, 0, null);
		if(!headerFile.IsValid())
		{
			fprintf(stderr, "Error opening header file, exiting\n");
			return 1;
		}
	}
	else
	{
		headerFile = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	Printer::SetHeaderFile(headerFile);

	string fileName = GetFilename(options[SOURCE].arg);

	if(!headerFile.IsValid())
	{
		fprintf(stderr, "Error opening header file for writing, exiting\n");
		return 1;
	}

	HLSLShader::OutputHeader(fileName.c_str());
	int shadersCompiled = 0;
	for(int i = 0; i < _countof(shaderTypes); ++i)
	{
		ShaderArgType &a = shaderTypes[i];
		if(options[a.arg])
		{
			if(!CompileFile(options[SOURCE].arg, options[a.arg].arg, options[SMVERSION].arg, a.type))
			{
				return 1;
			}
			++shadersCompiled;
		}
	}
	HLSLShader::OutputFooter();

	if(shadersCompiled == 0)
	{
		fprintf(stderr, "No shader types specified - output file will be useless!\n");
		return 1;
	}

	return 0;
}
