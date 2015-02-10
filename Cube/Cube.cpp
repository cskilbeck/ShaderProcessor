//////////////////////////////////////////////////////////////////////
// Viewport / Aspect Ratio
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

	gridVB.reset(new VertBuffer(806, null, DynamicUsage, Writeable));
	InputVertex *v = gridVB->Map(Context());
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
		*v++ = { { b, a, 0 }, cx };
		*v++ = { { t, a, 0 }, cx };
		*v++ = { { a, b, 0 }, cy };
		*v++ = { { a, t, 0 }, cy };
	}
	*v++ = { { 0, 0, 0 }, Color::Cyan };
	*v++ = { { 0, 0, 1000 }, Color::Cyan };
	gridVB->UnMap(Context());
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
	sampler.reset(new Sampler());

	psPhong->picTexture = texture.get();
	psPhong->tex1Sampler = sampler.get();

	cubeIndices.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage));
	cubeVerts.reset(new Shaders::Phong::VertBuffer(_countof(verts), verts, StaticUsage));

	psPhong->Light.lightPos = Float3(0, -15, 0);
	psPhong->Light.ambientColor = Float3(0.3f, 0.2f, 0.4f);
	psPhong->Light.diffuseColor = Float3(0.6f, 0.7f, 0.5f);
	psPhong->Light.specColor = Float3(1, 1, 0.8f);
	psPhong->Light.Commit(Context());

	MaterialOptions o;
	o.blend = BlendDisabled;
	blendDisabled.Create(o);

	o.blend = BlendEnabled;
	o.depthWrite = DepthWriteDisabled;
	blendEnabled.Create(o);

	vsUI.reset(new Shaders::UI::VS());
	psUI.reset(new Shaders::UI::PS());
	UIVerts.reset(new Shaders::UI::VertBuffer(6));
	uiTexture.reset(new Texture(TEXT("temp.png")));
	uiSampler.reset(new Sampler());

	MaterialOptions uiOptions;
	uiOptions.blend = BlendEnabled;
	uiOptions.depth = DepthDisabled;
	uiOptions.culling = CullingNone;
	uiOptions.fillMode = FillModeSolid;
	uiOptions.depthWrite = DepthWriteDisabled;
	uiMaterial.Create(uiOptions);

	vsUI->vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
	vsUI->vConstants.Commit(Context());
	psUI->pConstants.Color = Float4(1, 1, 1, 1);
	psUI->pConstants.Commit(Context());
	psUI->page = uiTexture.get();
	psUI->smplr = uiSampler.get();

	Show();

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDraw()
{
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

	if(Mouse::Pressed & Mouse::Button::Right)
	{
		camera.LookAt(cubePos);
	}

	if(Mouse::Held & Mouse::Button::Left)
	{
		Mouse::SetMode(Mouse::Mode::Captured, *this);
		camera.Rotate(Mouse::Delta.x * 0.001f, Mouse::Delta.y * 0.001f);
	}
	else
	{
		Mouse::SetMode(Mouse::Mode::Free, *this);
	}

	camera.Move(move);
	camera.Update();

	cubePos = Vec4(15, 15, 0);
	cubeScale = Vec4(5, 5, 5);
	cubeRot = Vec4(mFrame * 0.02f, mFrame * 0.01f, mFrame * 0.03f);

	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	blendDisabled.Activate(Context());

	Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(cubePos);
	vsPhong->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
	vsPhong->VertConstants.ModelMatrix = Transpose(modelMatrix);
	vsPhong->VertConstants.Commit(Context());

	psPhong->Camera.cameraPos = camera.position;
	psPhong->Camera.Commit(Context());

	vsPhong->Activate(Context());
	psPhong->Activate(Context());
	cubeVerts->Activate(Context());
	cubeIndices->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cubeIndices->Count(), 0, 0);

	modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
	modelMatrix *= TranslationMatrix(Vec4(psPhong->Light.lightPos));
	vsSolidColor->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
	vsSolidColor->VertConstants.Commit(Context());

	vsSolidColor->Activate(Context());
	psSolidColor->Activate(Context());
	octahedronVB->Activate(Context());
	octahedronIB->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(octahedronIB->Count(), 0, 0);

	vsSolidColor->VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix());
	vsSolidColor->VertConstants.Commit(Context());

	blendEnabled.Activate(Context());

	gridVB->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(gridVB->Count(), 0);

	drawList.SetMaterial(uiMaterial);
	drawList.SetShader(vsUI.get(), psUI.get(), UIVerts.get(), sizeof(Shaders::UI::InputVertex));
	drawList.BeginTriangleList();
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 0, 0, }, { 0, 0 } });
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 100, 100 }, { 1, 1 } });
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 0, 100 }, { 0, 1 } });
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 0, 0, }, { 0, 0 } });
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 100, 0 }, { 1, 0 } });
	drawList.AddVertex<Shaders::UI::InputVertex>({ { 100, 100 }, { 1, 1 } });
	drawList.End();
	drawList.Execute(Context());
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
	cubeVerts.reset();
	blendEnabled.Release();
	blendDisabled.Release();
}
