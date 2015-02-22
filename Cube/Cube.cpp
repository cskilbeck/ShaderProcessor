//////////////////////////////////////////////////////////////////////
// Fix where all the libs go (all in DX\) [DXBase, ShaderProcessor, DXGraphics, DX]
// Monitor resolution list/handle Alt-Enter
// Fix ViewMatrix Axes Y/Z up etc & mul(vert, matrix) thing
// Fix the font utility for once and good and proper (rewrite? in DX? Mesh fonts, Distance fields)
// Model hierarchy/Skinning shader
// General purpose shader for Assimp viewer
// 2D UI Elements/Scene
// SWF importer/converter/player!?
// Assimp
// Bullet
// Track resources and clean them up automatically (with warnings, like Font does)
//		Textures
//		Samplers
//		etc
// Start the long and laborious process of making it platform agnostic
//		ShaderProcessor (harumph, wtf to do about that? Cg?)
// ShaderProcessor
//		Structured Buffers/UAV support
//		Hull/Domain shader support
//		Instancing support / multiple vertex streams
//		Assembly Listing file
//		deal with multiple render targets
//		enable mode where bytecode not embedded
//		Shader Linking/Interfaces support...?
//		Nested structs
//		Sampler/Texture defaults
//		* documentation generator for shader #pragmas
//		* Syntax highlighting for .shader files
// * Fix the Event system (get rid of heap allocations, make it flexible)

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

MyDXWindow::MyDXWindow()
	: DXWindow(1280, 720, TEXT("Cube"), DepthBufferEnabled, Windowed),
	camera(this),
	dashCam(this)
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
	vector<Shaders::Colored::InputVertex> v;
	v.reserve(201 * 4 + 2);
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
		v.push_back({ { b, a, 0 }, cx });
		v.push_back({ { t, a, 0 }, cx });
		v.push_back({ { a, b, 0 }, cy });
		v.push_back({ { a, t, 0 }, cy });
	}
	v.push_back({ { 0, 0, 0 }, Color::Cyan });
	v.push_back({ { 0, 0, 1000 }, Color::Cyan });
	gridVB.reset(new Shaders::Colored::VertBuffer(806, v.data(), StaticUsage));
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

	Resized += [this] (WindowSizedEvent const &e)
	{
		camera.CalculatePerspectiveProjectionMatrix(0.5f, (float)ClientWidth() / ClientHeight());
		float w = renderTarget->FWidth();
		float h = renderTarget->FHeight();
		float r = (float)ClientWidth() - 10;
		float l = r - w;
		float t = 10;
		float b = t + h;
		Shaders::Blit::InputVertex *v = blitVB->Map(Context());
		v[0] = { { l, t }, { 0, 0 } };
		v[1] = { { r, t }, { 1, 0 } };
		v[2] = { { l, b }, { 0, 1 } };
		v[3] = { { r, b }, { 1, 1 } };
		blitVB->UnMap(Context());
		Shaders::Blit::VS &vs = blitShader->vs;
		vs.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
		vs.vConstants.Commit(Context());
	};

	FontManager::Init(this);
	font.reset(FontManager::Load(TEXT("Data\\debug")));

	coloredShader.reset(new Shaders::Colored());

	coloredVB.reset(new Shaders::Colored::VertBuffer(128, null, DynamicUsage, Writeable));

	CreateGrid();
	CreateOctahedron();

	cubeShader.reset(new Shaders::Phong());

	cubeTexture.reset(new Texture(TEXT("Data\\temp.jpg")));
	cubeSampler.reset(new Sampler());

	cubeShader->ps.picTexture = cubeTexture.get();
	cubeShader->ps.tex1Sampler = cubeSampler.get();

	cubeIndices.reset(new IndexBuffer<uint16>(_countof(indices), indices, StaticUsage));
	cubeVerts.reset(new Shaders::Phong::VertBuffer(_countof(verts), verts, StaticUsage));

	Shaders::Phong::PS &ps = cubeShader->ps;
	ps.Light.lightPos = Float3(0, -15, 0);
	ps.Light.ambientColor = Float3(0.3f, 0.2f, 0.4f);
	ps.Light.diffuseColor = Float3(0.6f, 0.7f, 0.5f);
	ps.Light.specColor = Float3(1, 1, 0.8f);
	ps.Light.Commit(Context());

	uiShader.reset(new Shaders::UI());
	UIVerts.reset(new Shaders::UI::VertBuffer(12, null, DynamicUsage, Writeable));
	uiTexture.reset(new Texture(TEXT("Data\\temp.png")));
	uiSampler.reset(new Sampler());

	uiShader->vs.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
	uiShader->vs.vConstants.Commit(Context());
	uiShader->ps.pConstants.Color = Float4(1, 1, 1, 1);
	uiShader->ps.pConstants.Commit(Context());
	uiShader->ps.page = uiTexture.get();
	uiShader->ps.smplr = uiSampler.get();

	spriteShader.reset(new Shaders::Sprite());
	spriteVerts.reset(new Shaders::Sprite::VertBuffer(2, null, DynamicUsage, Writeable));
	spriteTexture.reset(new Texture(TEXT("Data\\temp.jpg")));
	spriteSampler.reset(new Sampler());
	spriteShader->ps.smplr = spriteSampler.get();
	spriteShader->ps.page = spriteTexture.get();

	spriteSheet.reset(new SpriteSheet(TEXT("Data\\spriteSheet.xml")));

	renderTarget.reset(new RenderTarget(256, 256, RenderTarget::WithDepthBuffer));

	blitShader.reset(new Shaders::Blit());
	blitVB.reset(new Shaders::Blit::VertBuffer(4, null, DynamicUsage, Writeable));

	blitShader->ps.smplr = uiSampler.get();
	blitShader->ps.page = renderTarget.get();

	cubeShader->ps.Camera.cameraPos = dashCam.position;
	cubeShader->ps.Camera.Commit(Context());

	dashCam.CalculatePerspectiveProjectionMatrix(0.5f, 1.0f);
	dashCam.position = Vec4(0, 0, 0);
	dashCam.LookAt(Vec4(0, 1, 0));
	dashCam.Update();

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDraw()
{
	float deltaTime = (float)mTimer.Delta();
	float time = (float)mTimer.WallTime();

	float moveSpeed = 50.0f * deltaTime;
	float strafeSpeed = 50.0f * deltaTime;

	if(Keyboard::Held(VK_SHIFT))
	{
		moveSpeed *= 0.1f;
		strafeSpeed *= 0.1f;
	}

	Vec4f move(Vec4(0, 0, 0));
	if(Keyboard::Held('A')) { move += SetX(move, -strafeSpeed); }
	if(Keyboard::Held('D')) { move += SetX(move, strafeSpeed); }
	if(Keyboard::Held('W')) { move += SetY(move, moveSpeed); }
	if(Keyboard::Held('S')) { move += SetY(move, -moveSpeed); }
	if(Keyboard::Held('R')) { move += SetZ(move, strafeSpeed); }
	if(Keyboard::Held('F')) { move += SetZ(move, -strafeSpeed); }

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
	cubeRot = Vec4(time * 0.8f, time * 0.9f, time * 0.7f);

	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	{
		Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(cubePos);
		Shaders::Phong::VS &vs = cubeShader->vs;
		Shaders::Phong::PS &ps = cubeShader->ps;
		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
		vs.VertConstants.ModelMatrix = Transpose(modelMatrix);
		vs.VertConstants.Commit(Context());
		ps.Camera.cameraPos = camera.position;
		ps.Camera.Commit(Context());
		cubeShader->Activate(Context());
		cubeVerts->Activate(Context());
		cubeIndices->Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexed(cubeIndices->Count(), 0, 0);
	}

	{
		coloredShader->Activate(Context());

		Shaders::Colored::VS &vs = coloredShader->vs;

		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix());
		vs.VertConstants.Commit(Context());
		gridVB->Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Context()->Draw(gridVB->Count(), 0);

		Matrix modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
		modelMatrix *= TranslationMatrix(Vec4(cubeShader->ps.Light.lightPos));
		vs.VertConstants.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
		vs.VertConstants.Commit(Context());
		octahedronVB->Activate(Context());
		octahedronIB->Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexed(octahedronIB->Count(), 0, 0);
	}

	{
		drawList.Reset(Context(), uiShader.get(), UIVerts.get());

		float w = 100 + sinf(time * 0.9f) * 30;
		float h = 200 + sinf(time * 0.9f) * 30;
		float x0 = 0;
		float y0 = 0;
		float x1 = x0 + w;
		float y1 = y0 + h;

		Shaders::UI::PS::pConstants_t c;
		Shaders::UI::VS::vConstants_t v;
		v.TransformMatrix = Transpose(Camera::OrthoProjection2D(1920, 1080));
		c.Color = Float4(1, 1, 1, sinf(time) * 0.5f + 0.5f);
		drawList.SetConstantData(Vertex, v, Shaders::UI::VS::vConstants_index);
		drawList.SetConstantData(Pixel, c, Shaders::UI::PS::pConstants_index);
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

		w = 100 + sinf(time * 1.1f) * 30;
		h = 200 + sinf(time * 1.2f) * 30;
		x0 += 200;
		y0 += 200;
		x1 = x0 + w;
		y1 = y0 + h;

		c.Color = Float4(1, 1, 1, 1);
		drawList.SetConstantData(Pixel, c, uiShader->ps.pConstants.Index());
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

		{
			drawList.Reset(Context(), coloredShader.get(), coloredVB.get());
			Shaders::Colored::VS::VertConstants_t v;
			v.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
			drawList.SetConstantData(Vertex, v, 0);
			drawList.BeginTriangleStrip();
			using q = Shaders::Colored::InputVertex;
			drawList.AddVertex<q>({ { 0, 0, 0.5f }, 0x80000000 });
			drawList.AddVertex<q>({ { (float)ClientWidth(), 0, 0.5f }, 0x80000000 });
			drawList.AddVertex<q>({ { 0, 100, 0.5f }, 0x80000000 });
			drawList.AddVertex<q>({ { (float)ClientWidth(), 100, 0.5f }, 0x80000000 });
			drawList.End();

			Font::SetupDrawList(Context(), drawList, this);
			font->Begin();
			font->DrawString("Hello World", Vec2f(120, 440), Font::HLeft, Font::VTop, 2);
			font->DrawString("Hello #80FF00FF#World", Vec2f(220, 460));
			font->DrawString("Hello World", Vec2f(120, 340));
			font->DrawString(Format("DeltaTime % 8.2fms (% 3dfps)", deltaTime * 1000, (int)(1 / deltaTime)).c_str(), Vec2f(0, 0));
			font->End();

		}
		drawList.Execute();
	}

	spriteShader->gs.vConstants.TransformMatrix = Transpose(Camera::OrthoProjection2D(ClientWidth(), ClientHeight()));
	spriteShader->gs.vConstants.Commit(Context());
	Sprite *v = (Sprite *)spriteVerts->Map(Context());

	v[0].Position = { 320, 240 };
	v[0].Size = { 100, 100 };
	v[0].Color = 0xffffffff;
	v[0].Rotation = time;
	v[0].Pivot = { 0.5f, 0.5f };
	v[0].Scale = { 1, 1 };
	v[0].UVa = { 0.0f, 0.0f };
	v[0].UVb = { 1.0f, 1.0f };
	v[0].Flip = Sprite::Flips(1, 1);

	v[1].Position = { 520, 140 };
	v[1].Size = { 64, 64 };
	v[1].Color = 0xffffff00;
	v[1].Rotation = time * 0.9f;
	v[1].Pivot = { 0.5f, 0.5f };
	v[1].Scale = { sinf(time * 1.3f) * 0.25f + 0.75f, 1 };
	v[1].Flip = Sprite::Flips(0, 0);
	v[1].UVa = { 0.0f, 0.0f };
	v[1].UVb = { 1.0f, 1.0f };

	spriteVerts->UnMap(Context());
	spriteVerts->Activate(Context());
	spriteShader->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Context()->Draw(spriteVerts->Count(), 0);

	Sprite const &s = (*spriteSheet)["temp.png"];
	Sprite const &t = (*spriteSheet)["temp.jpg"];
	spriteSheet->SetupTransform(Context(), mClientWidth, mClientHeight);

	drawList.Reset(Context(), spriteSheet->mShader.get(), spriteSheet->mVertexBuffer.get());
	drawList.BeginPointList();
	Sprite &q = drawList.AddVertex<Sprite>();
	q.Set(t, { 600, 400 });
	q.Rotation = time * PI / 2;
	drawList.End();
	drawList.Execute();

	spriteSheet->BeginRun(Context());
	spriteSheet->Add(s,
					 { sinf(time * 2) * 200 + 200, cosf(time * 1.5f) * 200 + 200 },
					 { sinf(time * 1.75f) * 0.1f + 0.2f, cosf(time * 1.9f) * 0.1f + 0.2f },
					 { 0.5f, 0.5f },
					 0,
					 Color::White,
					 (mFrame >> 6) & 1, (mFrame >> 5) & 1);
	spriteSheet->ExecuteRun(Context());

	{
		Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(Vec4(0, 30, 0));
		//Matrix modelMatrix = ScaleMatrix(cubeScale) * TranslationMatrix(Vec4(0, 30, 0));
		Shaders::Phong::VS &vs = cubeShader->vs;
		vs.VertConstants.TransformMatrix = Transpose(dashCam.GetTransformMatrix(modelMatrix));
		vs.VertConstants.ModelMatrix = Transpose(modelMatrix);
		vs.VertConstants.Commit(Context());
	}

	renderTarget->Activate(Context());
	renderTarget->Clear(Context(), 0x208040);
	renderTarget->ClearDepth(Context(), DepthOnly);
	Viewport(0, 0, renderTarget->FWidth(), renderTarget->FHeight(), 0, 1).Activate(Context());
	cubeShader->Activate(Context());
	cubeVerts->Activate(Context());
	cubeIndices->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cubeIndices->Count(), 0, 0);

	ResetRenderTargetView();

	blitVB->Activate(Context());
	blitShader->Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Context()->Draw(blitVB->Count(), 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	coloredShader.reset();
	cubeShader.reset();
	cubeTexture.reset();
	cubeSampler.reset();
	cubeVerts.reset();

	uiShader.reset();
	UIVerts.reset();
	uiTexture.reset();
	uiSampler.reset();
	font.reset();

	spriteShader.reset();
	spriteVerts.reset();
	spriteSampler.reset();

	spriteSheet.reset();
}
