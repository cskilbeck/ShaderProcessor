//////////////////////////////////////////////////////////////////////
// Material (Shaders, Rasterizer, DepthStencil, Blend)
// Viewport

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

static Shaders::Phong::InputVertex verts[24] =
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

MyDXWindow::MyDXWindow() : DXWindow(640, 480, TEXT("Cube"), DepthBufferEnabled)
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

void MyDXWindow::CreateGrid()
{
	using namespace Shaders::Colored;
	vector<InputVertex> v;
	uint i = 0;
	float const b = -10;
	float const t = 10;
	InputVertex v1, v2, v3, v4;
	for(int x = -10; x <= 10; ++x)
	{
		float a = (float)x;
		Color cx = 0x80FFFFFF;
		Color cy = 0x80FFFFFF;
		if(x == 0)
		{
			cx = Color::BrightRed;
			cy = Color::BrightGreen;
		}
		v.push_back(v1 = { { a, b, 0 }, cy });
		v.push_back(v2 = { { a, t, 0 }, cy });
		v.push_back(v3 = { { b, a, 0 }, cx });
		v.push_back(v4 = { { t, a, 0 }, cx });
	}
	v.push_back(v1 = { { 0, 0, 0 }, Color::Cyan });
	v.push_back(v2 = { { 0, 0, 10 }, Color::Cyan });
	vbLineGrid.reset(new VertBuffer(v.size(), v.data()));
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::CreateOctahedron()
{
	using namespace Shaders::Colored;
	octahedron.reset(new VertBuffer(6));
	InputVertex *v = octahedron->Data();
	float f = sqrtf(2) / 2.0f;
	InputVertex t;
	v[0] = t = { {  0,  0,  1 }, 0xffffffff };
	v[1] = t = { { -f,  f,  0 }, 0xffffffff };
	v[2] = t = { {  f,  f,  0 }, 0xffffffff };
	v[3] = t = { {  f, -f,  0 }, 0xffffffff };
	v[4] = t = { { -f, -f,  0 }, 0xffffffff };
	v[5] = t = { {  0,  0, -1 }, 0xffffffff };
	octahedron->Commit(Context());

	octahedronIB.reset(new IndexBuffer<uint16>(3 * 8));
	uint16 *o = octahedronIB->Data();
	o[ 0] = 0; o[ 1] = 2; o[ 2] = 1;
	o[ 3] = 0; o[ 4] = 3; o[ 5] = 2;
	o[ 6] = 0; o[ 7] = 4; o[ 8] = 3;
	o[ 9] = 0; o[10] = 1; o[11] = 4;
	o[12] = 5; o[13] = 1; o[14] = 2;
	o[15] = 5; o[16] = 2; o[17] = 3;
	o[18] = 5; o[19] = 3; o[20] = 4;
	o[21] = 5; o[22] = 4; o[23] = 1;
	octahedronIB->Commit(Context());
}

//////////////////////////////////////////////////////////////////////

bool MyDXWindow::OnCreate()
{
	if(!DXWindow::OnCreate())
	{
		return false;
	}

	vsLine.reset(new Shaders::Colored::VS());
	psLine.reset(new Shaders::Colored::PS());

	CreateGrid();
	CreateOctahedron();

	vs.reset(new Shaders::Phong::VS());
	ps.reset(new Shaders::Phong::PS());

	texture.reset(new Texture(TEXT("temp.jpg")));
	ps->picTexture = texture.get();

	sampler.reset(new Sampler());
	ps->tex1Sampler = sampler.get();

	indexBuffer.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage, NotCPUAccessible));
	vertexBuffer.reset(new Shaders::Phong::VertBuffer(_countof(verts), verts));

	ps->Light.lightPos = Float3(0, -5, 0);
	ps->Light.ambientColor = Float3(0.2f, 0.2f, 0.2f);
	ps->Light.diffuseColor = Float3(0.8f, 0.8f, 0.8f);
	ps->Light.specColor = Float3(1, 1, 1);
	ps->Light.Commit(Context());

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	DXB(Device()->CreateRasterizerState(&rasterizerDesc, &rasterizerState));

	CD3D11_DEPTH_STENCIL_DESC depthstencilDesc(D3D11_DEFAULT);
	DXB(Device()->CreateDepthStencilState(&depthstencilDesc, &depthStencilState));

	CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
	DXB(Device()->CreateBlendState(&blendDesc, &blendState));

	Show();

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDraw()
{
	Clear(Color(32, 64, 128));
	ClearDepth(DepthClearOption::Depth, 1.0f, 0);

	float moveSpeed = 0.1f;
	float strafeSpeed = 0.1f;

	Vec4f move(Vec4(0, 0, 0));
	if(Keyboard::Held('W')) { move = SetY(move, moveSpeed); }
	if(Keyboard::Held('S')) { move = SetY(move, -moveSpeed); }
	if(Keyboard::Held('A')) { move = SetX(move, -strafeSpeed); }
	if(Keyboard::Held('D')) { move = SetX(move, strafeSpeed); }
	if(Keyboard::Held('R')) { move = SetZ(move, strafeSpeed); }
	if(Keyboard::Held('F')) { move = SetZ(move, -strafeSpeed); }

	camera.Move(move);

	if(Mouse::Held & Mouse::Button::Left)
	{
		Mouse::SetMode(Mouse::Mode::Captured, *this);
		camera.Rotate(Mouse::Delta.x * 0.001f, Mouse::Delta.y * 0.001f);
	}
	else
	{
		Mouse::SetMode(Mouse::Mode::Free, *this);
	}

	if(Mouse::Pressed & Mouse::Button::Right)
	{
		camera.LookAt(Vec4(4, 4, 2));
	}

	camera.Update();

	Context()->RSSetState(rasterizerState);
	Context()->OMSetBlendState(blendState, null, 0xffffffff);
	Context()->OMSetDepthStencilState(depthStencilState, 0);


	Matrix modelMatrix = RotationMatrix(mFrame * 0.02f, mFrame * 0.01f, mFrame * 0.03f);
	modelMatrix *= TranslationMatrix(Vec4(4, 4, 0));

	CopyMatrix(&vs->VertConstants.TransformMatrix, TransposeMatrix(camera.GetTransformMatrix(modelMatrix)));
	CopyMatrix(&vs->VertConstants.ModelMatrix, TransposeMatrix(modelMatrix));
	vs->VertConstants.Commit(Context());
	ps->Camera.cameraPos = camera.position;
	ps->Camera.Commit(Context());

	vs->Activate(Context());
	ps->Activate(Context());
	vertexBuffer->Activate(Context());
	indexBuffer->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(_countof(indices), 0, 0);

	CopyMatrix(&vsLine->VertConstants.TransformMatrix, TransposeMatrix(camera.GetTransformMatrix()));
	vsLine->VertConstants.Commit(Context());

	vsLine->Activate(Context());
	psLine->Activate(Context());
	vbLineGrid->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(vbLineGrid->Count(), 0);

	modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
	modelMatrix *= TranslationMatrix(Vec4(ps->Light.lightPos));
	CopyMatrix(&vsLine->VertConstants.TransformMatrix, TransposeMatrix(camera.GetTransformMatrix(modelMatrix)));
	vsLine->VertConstants.Commit(Context());

	octahedron->Activate(Context());
	octahedronIB->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(octahedronIB->Count(), 0, 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	TRACE("OnDestroy!\n");
	vsLine.reset();
	psLine.reset();
	vs.reset();
	ps.reset();
	texture.reset();
	sampler.reset();
	vertexBuffer.reset();
	rasterizerState.Release();
	depthStencilState.Release();
	blendState.Release();
}
