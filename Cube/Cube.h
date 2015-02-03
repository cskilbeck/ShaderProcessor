//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	Camera camera;
	Matrix worldMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	Ptr<vs_shader> vertexShader;
	Ptr<ps_shader> pixelShader;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;
	Ptr<IndexBuffer<uint16>> indexBuffer;
	Ptr<vs_shader::VertexBuffer> vertexBuffer;
	DXPtr<ID3D11RasterizerState> rasterizerState;
	DXPtr<ID3D11DepthStencilState>	depthStencilState;
	DXPtr<ID3D11BlendState>	blendState;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;
};

