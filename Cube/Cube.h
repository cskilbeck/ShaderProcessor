//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	FPSCamera camera;

	Ptr<Shaders::Phong::VS> vs;
	Ptr<Shaders::Phong::PS> ps;
	Ptr<Shaders::Phong::VertBuffer> vertexBuffer;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;
	Ptr<IndexBuffer<uint16>> indexBuffer;

	Ptr<Shaders::Colored::VS> vsLine;
	Ptr<Shaders::Colored::PS> psLine;
	Ptr<Shaders::Colored::VertBuffer> vbLineGrid;
	Ptr<Shaders::Colored::VertBuffer> octahedron;
	Ptr<IndexBuffer<uint16>> octahedronIB;

	DXPtr<ID3D11RasterizerState> rasterizerState;
	DXPtr<ID3D11DepthStencilState>	depthStencilState;
	DXPtr<ID3D11BlendState>	blendState;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;

	void CreateGrid();
	void CreateOctahedron();
};

