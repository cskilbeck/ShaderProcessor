//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\ShaderProcessor\output.h"

//////////////////////////////////////////////////////////////////////

static vs_shader::InputVertex verts[24] =
{
	Float3(-1, +1, +1), Half2(0, 0), Color::White, Float3(0, 0, +1),// 00
	Float3(+1, +1, +1), Half2(1, 0), Color::White, Float3(0, 0, +1),// 01
	Float3(+1, -1, +1), Half2(1, 1), Color::White, Float3(0, 0, +1),// 02
	Float3(-1, -1, +1), Half2(0, 1), Color::White, Float3(0, 0, +1),// 03

	Float3(-1, -1, +1), Half2(0, 0), Color::White, Float3(0, -1, 0),// 04
	Float3(+1, -1, +1), Half2(1, 0), Color::White, Float3(0, -1, 0),// 05
	Float3(+1, -1, -1), Half2(1, 1), Color::White, Float3(0, -1, 0),// 06
	Float3(-1, -1, -1), Half2(0, 1), Color::White, Float3(0, -1, 0),// 07

	Float3(-1, -1, -1), Half2(0, 0), Color::White, Float3(0, 0, -1),// 08
	Float3(+1, -1, -1), Half2(1, 0), Color::White, Float3(0, 0, -1),// 09
	Float3(+1, +1, -1), Half2(1, 1), Color::White, Float3(0, 0, -1),// 10
	Float3(-1, +1, -1), Half2(0, 1), Color::White, Float3(0, 0, -1),// 11

	Float3(-1, +1, -1), Half2(0, 0), Color::White, Float3(0, +1, 0),// 12
	Float3(+1, +1, -1), Half2(1, 0), Color::White, Float3(0, +1, 0),// 13
	Float3(+1, +1, +1), Half2(1, 1), Color::White, Float3(0, +1, 0),// 14
	Float3(-1, +1, +1), Half2(0, 1), Color::White, Float3(0, +1, 0),// 15

	Float3(+1, +1, +1), Half2(0, 0), Color::White, Float3(+1, 0, 0),// 16
	Float3(+1, +1, -1), Half2(1, 0), Color::White, Float3(+1, 0, 0),// 17
	Float3(+1, -1, -1), Half2(1, 1), Color::White, Float3(+1, 0, 0),// 18
	Float3(+1, -1, +1), Half2(0, 1), Color::White, Float3(+1, 0, 0),// 19

	Float3(-1, +1, -1), Half2(0, 0), Color::White, Float3(-1, 0, 0),// 20
	Float3(-1, +1, +1), Half2(1, 0), Color::White, Float3(-1, 0, 0),// 21
	Float3(-1, -1, +1), Half2(1, 1), Color::White, Float3(-1, 0, 0),// 22
	Float3(-1, -1, -1), Half2(0, 1), Color::White, Float3(-1, 0, 0),// 23
};

//////////////////////////////////////////////////////////////////////

static uint16 indices[36] =
{
	 0, 2, 1, 0, 3, 2,
	 4, 6, 5, 4, 7, 6,
	 8,10, 9, 8,11,10,
	12,14,13,12,15,14,
	16,18,17,16,19,18,
	20,22,21,20,23,22
};

//////////////////////////////////////////////////////////////////////

MyDXWindow::MyDXWindow() : DXWindow(640, 480)
{
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnKeyDown(int key, uintptr flags)
{
	if(key == 27)
	{
		Close();
	}
}

//////////////////////////////////////////////////////////////////////

bool MyDXWindow::OnCreate()
{
	if(!DXWindow::OnCreate())
	{
		return false;
	}

	rollPitchYaw = Vec3(0, 0, 0);

	vertexShader.reset(new vs_shader());
	pixelShader.reset(new ps_shader());
	texture.reset(new Texture(TEXT("temp.jpg")));
	sampler.reset(new Sampler());
	pixelShader->tex1Sampler = sampler.get();
	pixelShader->picTexture = texture.get();

	indexBuffer.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage, NotCPUAccessible));
	vertexBuffer.reset(new vs_shader::VertexBuffer(_countof(verts), verts));

	camera.CalculatePerspectiveProjectionMatrix();
	camera.position = Vec4(0, -2, 0);
	camera.Update();

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	DXB(Device()->CreateRasterizerState(&rasterizerDesc, &rasterizerState));

	CD3D11_DEPTH_STENCIL_DESC depthstencilDesc(D3D11_DEFAULT);
	DXB(Device()->CreateDepthStencilState(&depthstencilDesc, &depthStencilState));

	CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	DXB(Device()->CreateBlendState(&blendDesc, &blendState));

	Mouse::SetMode(Mouse::Mode::Captured, *this);

	Show();

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDraw()
{
	Clear(Color(32, 64, 128));
	ClearDepth(DepthClearOption::Depth, 0.0f, 0);

	camera.pitch -= Mouse::Delta.y * 0.001f;
	camera.yaw -= Mouse::Delta.x * 0.001f;

	float moveSpeed = 0.1f;
	float strafeSpeed = 0.1f;

	Vec4f move(Vec4(0, 0, 0));
	if(Keyboard::Held('W')) { move = SetZ(move, moveSpeed); }
	if(Keyboard::Held('S')) { move = SetZ(move, -moveSpeed); }
	if(Keyboard::Held('A')) { move = SetX(move, -strafeSpeed); }
	if(Keyboard::Held('D')) { move = SetX(move, strafeSpeed); }
	if(Keyboard::Held('R')) { move = SetY(move, strafeSpeed); }
	if(Keyboard::Held('F')) { move = SetY(move, -strafeSpeed); }

	camera.Move(move);

	if(Mouse::Pressed & Mouse::Button::Right)
	{
		camera.LookAt(Vec4(0, 2, 0));
	}

	camera.Update();

	rollPitchYaw.x = mFrame * 0.04f;
	rollPitchYaw.y = mFrame * 0.01f;
	rollPitchYaw.z = mFrame * 0.03f;
	Matrix modelMatrix = DirectX::XMMatrixRotationRollPitchYaw(rollPitchYaw.x, rollPitchYaw.y, rollPitchYaw.z);
	modelMatrix *= TranslationMatrix(Vec4(0, 2, 0));

	Matrix t = TransposeMatrix(camera.GetTransformMatrix(modelMatrix));
	memcpy(&vertexShader->VertConstants.TransformMatrix, &t, sizeof(Matrix));

	Matrix m = TransposeMatrix(modelMatrix);
	memcpy(&vertexShader->VertConstants.ModelMatrix, &m, sizeof(Matrix));

	vertexShader->VertConstants.Commit(Context());

	pixelShader->ColourStuff.cameraPos = Float3(0, -10, 0);
	pixelShader->ColourStuff.lightPos = Float3(10, -5, 10);
	pixelShader->ColourStuff.ambientColor = Float3(0.2f, 0.2f, 0.2f);
	pixelShader->ColourStuff.diffuseColor = Float3(0.5f, 0.5f, 0.5f);
	pixelShader->ColourStuff.specColor = Float3(1, 1, 1);

	pixelShader->ColourStuff.Commit(Context());

	vertexShader->Activate(Context());
	pixelShader->Activate(Context());
	vertexBuffer->Activate(Context());
	indexBuffer->Activate(Context());

	Context()->RSSetState(rasterizerState);
	Context()->OMSetBlendState(blendState, null, 0xffffffff);
	Context()->OMSetDepthStencilState(depthStencilState, 0);

	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(_countof(indices), 0, 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	vertexShader.reset();
	pixelShader.reset();
	texture.reset();
	sampler.reset();
	vertexBuffer.reset();
	rasterizerState.Release();
	depthStencilState.Release();
	blendState.Release();
}
