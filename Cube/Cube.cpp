//////////////////////////////////////////////////////////////////////
// Aspect Ratio
// SpriteList
// Font
// Fix ViewMatrix Axes Y/Z up etc

// Split up DX.lib:
// DXBase: enough for ShaderProcessor
// DXUtil: everything else
// DX: DXBase + DXUtil

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
	using namespace Shaders;

	gridVB.reset(new Colored::VertBuffer(806, null, DynamicUsage, Writeable));
	Colored::InputVertex *v = gridVB->Map(Context());
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
	using namespace Shaders;

	octahedronVB.reset(new Colored::VertBuffer(6));
	Colored::InputVertex *v = octahedronVB->Data();
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

	//font = FontManager::Load(TEXT("data\\fixedsys"));
	font = null;

	coloredShader.reset(new Shaders::Colored());

	Shaders::Colored c;
	c.VertexShader.VertConstants.Commit(Context());
	c.Activate(Context());

	CreateGrid();
	CreateOctahedron();

	phongShader.reset(new Shaders::Phong());

	texture.reset(new Texture(TEXT("temp.jpg")));
	sampler.reset(new Sampler());

	phongShader->PixelShader.picTexture = texture.get();
	phongShader->PixelShader.tex1Sampler = sampler.get();

	cubeIndices.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage));
	cubeVerts.reset(new Shaders::Phong::VertBuffer(_countof(verts), verts, StaticUsage));

	phongShader->PixelShader.Light.lightPos = Float3(0, -15, 0);
	phongShader->PixelShader.Light.ambientColor = Float3(0.3f, 0.2f, 0.4f);
	phongShader->PixelShader.Light.diffuseColor = Float3(0.6f, 0.7f, 0.5f);
	phongShader->PixelShader.Light.specColor = Float3(1, 1, 0.8f);
	phongShader->PixelShader.Light.Commit(Context());

	MaterialOptions o;
	o.blend = BlendDisabled;
	blendDisabled.Create(o);

	o.blend = BlendEnabled;
	o.depthWrite = DepthWriteDisabled;
	blendEnabled.Create(o);

	MaterialOptions uiOptions;

	uiShader.reset(new Shaders::UI());
	UIVerts.reset(new Shaders::UI::VertBuffer(12));
	uiTexture.reset(new Texture(TEXT("temp.png")));
	uiSampler.reset(new Sampler());
	uiOptions.blend = BlendEnabled;
	uiOptions.depth = DepthDisabled;
	uiOptions.culling = CullingNone;
	uiOptions.fillMode = FillModeSolid;
	uiOptions.depthWrite = DepthWriteDisabled;
	uiMaterial.Create(uiOptions);

	uiShader->VertexShader.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
	uiShader->VertexShader.vConstants.Commit(Context());
	uiShader->PixelShader.pConstants.Color = Float4(1, 1, 1, 1);
	uiShader->PixelShader.pConstants.Commit(Context());
	uiShader->PixelShader.page = uiTexture.get();
	uiShader->PixelShader.smplr = uiSampler.get();

	spriteShader.reset(new Shaders::Sprite());
	spriteVerts.reset(new Shaders::Sprite::VertBuffer(2));

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

	{
		Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(cubePos);
		Shaders::Phong::VS &vs = phongShader->VertexShader;
		Shaders::Phong::PS &ps = phongShader->PixelShader;
		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
		vs.VertConstants.ModelMatrix = Transpose(modelMatrix);
		vs.VertConstants.Commit(Context());
		ps.Camera.cameraPos = camera.position;
		ps.Camera.Commit(Context());
		phongShader->Activate(Context());
		cubeVerts->Activate(Context());
		cubeIndices->Activate(Context());
		blendDisabled.Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexed(cubeIndices->Count(), 0, 0);
	}

	{
		Shaders::Colored::VS &vs = coloredShader->VertexShader;
		Matrix modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
		modelMatrix *= TranslationMatrix(Vec4(phongShader->PixelShader.Light.lightPos));
		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
		vs.VertConstants.Commit(Context());
		coloredShader->Activate(Context());
		octahedronVB->Activate(Context());
		octahedronIB->Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexed(octahedronIB->Count(), 0, 0);
		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix());
		vs.VertConstants.Commit(Context());
		blendEnabled.Activate(Context());
		gridVB->Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Context()->Draw(gridVB->Count(), 0);
	}

	{
		drawList.SetShader(uiShader.get(), UIVerts.get(), sizeof(Shaders::UI::InputVertex));
		drawList.SetMaterial(uiMaterial);

		float w = 100 + sinf(mFrame * 0.2f) * 30;
		float h = 200 + sinf(mFrame * 0.3f) * 30;
		float x0 = 0;
		float y0 = 0;
		float x1 = x0 + w;
		float y1 = y0 + h;

		Shaders::UI::PS::pConstants_t c;
		c.Color = Float4(1, 1, 1, sinf((float)mFrame * 0.2f) * 0.5f + 0.5f);
		drawList.SetPSConstantData(c, uiShader->PixelShader.pConstants.Index());
		{
			using v = Shaders::UI::InputVertex;
			drawList.BeginTriangleList();
			drawList.AddVertex<v>({ { x0, y0, }, { 0, 0 } });
			drawList.AddVertex<v>({ { x1, y0 }, { 1, 0 } });
			drawList.AddVertex<v>({ { x0, y1 }, { 0, 1 } });
			drawList.AddVertex<v>({ { x1, y0, }, { 1, 0 } });
			drawList.AddVertex<v>({ { x1, y1 }, { 1, 1 } });
			drawList.AddVertex<v>({ { x0, y1 }, { 0, 1 } });
			drawList.End();
		}

		w = 100 + sinf(mFrame * 0.1f) * 30;
		h = 200 + sinf(mFrame * 0.4f) * 30;
		x0 += 200;
		y0 += 200;
		x1 = x0 + w;
		y1 = y0 + h;

		c.Color = Float4(1, 1, 1, 1);
		drawList.SetPSConstantData(c, uiShader->PixelShader.pConstants.Index());
		{
			using v = Shaders::UI::InputVertex;
			drawList.BeginTriangleList();
			drawList.AddVertex<v>({ { x0, y0, }, { 0, 0 } });
			drawList.AddVertex<v>({ { x1, y0 }, { 1, 0 } });
			drawList.AddVertex<v>({ { x0, y1 }, { 0, 1 } });
			drawList.AddVertex<v>({ { x1, y0, }, { 1, 0 } });
			drawList.AddVertex<v>({ { x1, y1 }, { 1, 1 } });
			drawList.AddVertex<v>({ { x0, y1 }, { 0, 1 } });
			drawList.End();
		}
		drawList.Execute(Context());
	}

	spriteShader->GeometryShader.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
	spriteShader->GeometryShader.vConstants.Commit(Context());
	spriteShader->Activate(Context());
	Shaders::Sprite::InputVertex *v = spriteVerts->Data();
	v[0].Position = { 320, 240 }; v[0].Size = { 50, 100 }; v[0].Color = 0xffffffff;
	v[1].Position = { 520, 140 }; v[1].Size = { 100, 100 }; v[1].Color = 0xff0000ff;
	spriteVerts->Commit(Context());
	spriteVerts->Activate(Context());
	spriteShader->Activate(Context());
	uiMaterial.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Context()->Draw(spriteVerts->Count(), 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	coloredShader.reset();
	phongShader.reset();
	texture.reset();
	sampler.reset();
	cubeVerts.reset();
	blendEnabled.Release();
	blendDisabled.Release();

	uiShader.reset();
	UIVerts.reset();
	uiTexture.reset();
	uiSampler.reset();
	uiMaterial.Release();
	Release(font);

	spriteShader.reset();
	spriteVerts.reset();
}
