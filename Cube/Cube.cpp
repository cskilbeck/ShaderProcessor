//////////////////////////////////////////////////////////////////////
// Viewport
// DrawList
// SpriteList
// Font
// Fix ViewMatrix Axes Y/Z up etc

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
	 0, 1, 2, 0, 2, 3,
	 4, 5, 6, 4, 6, 7,
	 8, 9,10, 8,10,11,
	12,13,14,12,14,15,
	16,17,18,16,18,19,
	20,21,22,20,22,23
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
	InputVertex i;
	for(int x = -1000; x <= 1000; x += 10)
	{
		float a = (float)x;
		float b = -1000;
		float t = 1000;
		Color cx = 0x40FFFFFF;
		Color cy = 0x40FFFFFF;
		if(x == 0)
		{
			cx = Color::BrightRed;
			cy = Color::BrightGreen;
		}
		v.push_back(i = { { b, a, 0 }, cx });
		v.push_back(i = { { t, a, 0 }, cx });
		v.push_back(i = { { a, b, 0 }, cy });
		v.push_back(i = { { a, t, 0 }, cy });
	}
	v.push_back(i = { { 0, 0, 0 }, Color::Cyan });
	v.push_back(i = { { 0, 0, 1000 }, Color::Cyan });
	gridVB.reset(new VertBuffer(v.size(), v.data()));
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::CreateOctahedron()
{
	using namespace Shaders::Colored;
	octahedronVB.reset(new VertBuffer(6));
	InputVertex *v = octahedronVB->Data();
	float f = sqrtf(2) / 2.0f;
	v[0] = { {  0,  0,  1 }, 0xffffffff };
	v[1] = { { -f,  f,  0 }, 0xffffffff };
	v[2] = { {  f,  f,  0 }, 0xffffffff };
	v[3] = { {  f, -f,  0 }, 0xffffffff };
	v[4] = { { -f, -f,  0 }, 0xffffffff };
	v[5] = { {  0,  0, -1 }, 0xffffffff };
	octahedronVB->Commit(Context());

	octahedronIB.reset(new IndexBuffer<uint16>(3 * 8));
	uint16 *o = octahedronIB->Data();
	o[ 0] = 0; o[ 1] = 1; o[ 2] = 2;
	o[ 3] = 0; o[ 4] = 2; o[ 5] = 3;
	o[ 6] = 0; o[ 7] = 3; o[ 8] = 4;
	o[ 9] = 0; o[10] = 4; o[11] = 1;
	o[12] = 5; o[13] = 2; o[14] = 1;
	o[15] = 5; o[16] = 3; o[17] = 2;
	o[18] = 5; o[19] = 4; o[20] = 3;
	o[21] = 5; o[22] = 1; o[23] = 4;
	octahedronIB->Commit(Context());
}

//////////////////////////////////////////////////////////////////////

bool MyDXWindow::OnCreate()
{
	if(!DXWindow::OnCreate())
	{
		return false;
	}

	vsSolidColor.reset(new Shaders::Colored::VS());
	psSolidColor.reset(new Shaders::Colored::PS());

	CreateGrid();
	CreateOctahedron();

	vsPhong.reset(new Shaders::Phong::VS());
	psPhong.reset(new Shaders::Phong::PS());

	texture.reset(new Texture(TEXT("temp.jpg")));
	psPhong->picTexture = texture.get();

	sampler.reset(new Sampler());
	psPhong->tex1Sampler = sampler.get();

	indexBuffer.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage, NotCPUAccessible));
	vbPhong.reset(new Shaders::Phong::VertBuffer(_countof(verts), verts));

	psPhong->Light.lightPos = Float3(0, -15, 0);
	psPhong->Light.ambientColor = Float3(0.2f, 0.2f, 0.2f);
	psPhong->Light.diffuseColor = Float3(0.8f, 0.8f, 0.8f);
	psPhong->Light.specColor = Float3(1, 1, 1);
	psPhong->Light.Commit(Context());

	MaterialOptions o;
	o.blend = BlendDisabled;
	blendDisabled.Create(o);

	o.blend = BlendEnabled;
	o.depthWrite = DepthWriteDisabled;
	blendEnabled.Create(o);

	Show();

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDraw()
{
	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	float moveSpeed = 1.0f;
	float strafeSpeed = 1.0f;

	if(Keyboard::Held(VK_SHIFT))
	{
		moveSpeed *= 0.1f;
		strafeSpeed *= 0.1f;
	}

	Vec4f move(Vec4(0, 0, 0));
	if(Keyboard::Held('A')) { move = SetX(move, -strafeSpeed); }
	if(Keyboard::Held('D')) { move = SetX(move, strafeSpeed); }
	if(Keyboard::Held('W')) { move = SetY(move, moveSpeed); }
	if(Keyboard::Held('S')) { move = SetY(move, -moveSpeed); }
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
		camera.LookAt(Vec4(4, 4, 0));
	}

	camera.Update();

	blendDisabled.Activate(Context());

	Matrix modelMatrix = RotationMatrix(mFrame * 0.02f, mFrame * 0.01f, mFrame * 0.03f);
	modelMatrix *= ScaleMatrix(Vec4(5, 5, 5)) * TranslationMatrix(Vec4(4, 4, 0));

	vsPhong->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
	vsPhong->VertConstants.ModelMatrix = Transpose(modelMatrix);
	vsPhong->VertConstants.Commit(Context());

	psPhong->Camera.cameraPos = camera.position;
	psPhong->Camera.Commit(Context());

	vsPhong->Activate(Context());
	psPhong->Activate(Context());

	vbPhong->Activate(Context());
	indexBuffer->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(indexBuffer->Count(), 0, 0);

	vsSolidColor->Activate(Context());
	psSolidColor->Activate(Context());

	modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
	modelMatrix *= TranslationMatrix(Vec4(psPhong->Light.lightPos));
	vsSolidColor->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
	vsSolidColor->VertConstants.Commit(Context());

	octahedronVB->Activate(Context());
	octahedronIB->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(octahedronIB->Count(), 0, 0);

	blendEnabled.Activate(Context());

	vsSolidColor->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix());
	vsSolidColor->VertConstants.Commit(Context());

	gridVB->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(gridVB->Count(), 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	vsSolidColor.reset();
	psSolidColor.reset();
	vsPhong.reset();
	psPhong.reset();
	texture.reset();
	sampler.reset();
	vbPhong.reset();
	blendEnabled.Release();
	blendDisabled.Release();
}
