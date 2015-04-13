//////////////////////////////////////////////////////////////////////
// RTCB001: DON'T RETURN A BOOL TO INDICATE SUCCESS or FAILURE (return a signed integer, where a negative value indicates failure)
// RTCB002: DON'T DO ANYTHING THAT CAN FAIL IN A CONSTRUCTOR (unless you dig exceptions, in which case throw up all you like)
// RTCB003: WHEN DECLARING FUNCTION PARAMETERS, PREFER enum TO bool
// RTCB004: 

// Bullet
// Normal mapping
// Shadow mapping
// Track resources and clean them up automatically (with warnings, like Font does)
//		Textures
//		Samplers
//		etc
// 2D UI Elements/SceneGraph
// zLib: support Deflate64
//		Test the AssetManager
// ?? Shader de-duplication?
// Proper logging instead of a janky handful of macros
// Debug text
// Clean up the FileBase/DiskFile/MemoryFile/WinResource/FileResource/Resource/Blob mess
// Pick a base aspect ratio and do the right thing when the window is resized
// Fix where all the libs go (all in DX\) [DXBase, ShaderProcessor, DXGraphics, DX]
// Monitor resolution list/handle Alt-Enter
// Fix the font utility for once and good and proper (rewrite? in DX? Mesh fonts, Distance fields)
// Model hierarchy/Skinning shader/Anim player
// SWF importer/converter/player!?
// Assimp
//		Default shader
//		Preprocessor / native loader
//		Shared vert/index buffers across meshes
//		Named shader support
//		Skeleton/Weights
//		* Override IOSystem/IOStream with AssetManager
// Start the long and laborious process of making it platform agnostic
//		ShaderProcessor (harumph, wtf to do about that? Cg? GLSL? PSSL? MeTal?)
// Spock
//		Make debug info default to on in Debug builds
// ShaderProcessor
//		Error check everything
//		Structured Buffers/UAV support
//		Hull/Domain/Compute shader support (Tesselate the duck to buggery! How do you calc the patches?)
//		Assembly Listing file
//		deal with multiple render targets
//		Sort out Resource types (Texture1D/2D/3D etc)
//		Support all shader types & compute
//		support Buffers<>
//		deal with anonymous cbuffers and tbuffers (index instead of name)
//		test alignment/padding etc
//		deal with Buffers of structs (no padding)
//		\ Shader Linking/Interfaces support...?
//		\ Sampler/Texture defaults
//		* Allow row or column major matrices
//		* Instancing support / multiple vertex streams
//		* Nested structs (names of the structs!?)
//		* Fix the crappy #pragma renderstate system (run CL /P beforehand - what about #includes though?)
//		* Honour the input binding slots specified (and deal with non-contiguous ones)
//		* Identify matrices in input layout
//		* Use Map/UnMap for const buffers
//		* enable mode where bytecode not embedded but loaded from a file
//		* documentation generator for shader #pragmas
//		* Syntax highlighting for .shader files
//		* Fix MSBuild Spock dependency bug (building everything)
// * Fix the Event system (get rid of heap allocations, make it flexible)
// * Fix ViewMatrix Axes Y/Z up etc & mul(vert, matrix) thing (left/right handed)
// * Move some things into Matrix from Camera
// * Fix Shader/ShaderBase constructor

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

static Shaders::Phong::InputVertex verts[24] =
{
	{ { -1, +1, +1 }, { 0, 0 }, 0xffffffff, {  0,  0, +1 } },// 00
	{ { +1, +1, +1 }, { 1, 0 }, 0xffffffff, {  0,  0, +1 } },// 01
	{ { +1, -1, +1 }, { 1, 1 }, 0xffffffff, {  0,  0, +1 } },// 02
	{ { -1, -1, +1 }, { 0, 1 }, 0xffffffff, {  0,  0, +1 } },// 03
	{ { -1, -1, +1 }, { 0, 0 }, 0xffffffff, {  0, -1,  0 } },// 04
	{ { +1, -1, +1 }, { 1, 0 }, 0xffffffff, {  0, -1,  0 } },// 05
	{ { +1, -1, -1 }, { 1, 1 }, 0xffffffff, {  0, -1,  0 } },// 06
	{ { -1, -1, -1 }, { 0, 1 }, 0xffffffff, {  0, -1,  0 } },// 07
	{ { -1, -1, -1 }, { 0, 0 }, 0xffffffff, {  0,  0, -1 } },// 08
	{ { +1, -1, -1 }, { 1, 0 }, 0xffffffff, {  0,  0, -1 } },// 09
	{ { +1, +1, -1 }, { 1, 1 }, 0xffffffff, {  0,  0, -1 } },// 10
	{ { -1, +1, -1 }, { 0, 1 }, 0xffffffff, {  0,  0, -1 } },// 11
	{ { -1, +1, -1 }, { 0, 0 }, 0xffffffff, {  0, +1,  0 } },// 12
	{ { +1, +1, -1 }, { 1, 0 }, 0xffffffff, {  0, +1,  0 } },// 13
	{ { +1, +1, +1 }, { 1, 1 }, 0xffffffff, {  0, +1,  0 } },// 14
	{ { -1, +1, +1 }, { 0, 1 }, 0xffffffff, {  0, +1,  0 } },// 15
	{ { +1, +1, +1 }, { 0, 0 }, 0xffffffff, { +1,  0,  0 } },// 16
	{ { +1, +1, -1 }, { 1, 0 }, 0xffffffff, { +1,  0,  0 } },// 17
	{ { +1, -1, -1 }, { 1, 1 }, 0xffffffff, { +1,  0,  0 } },// 18
	{ { +1, -1, +1 }, { 0, 1 }, 0xffffffff, { +1,  0,  0 } },// 19
	{ { -1, +1, -1 }, { 0, 0 }, 0xffffffff, { -1,  0,  0 } },// 20
	{ { -1, +1, +1 }, { 1, 0 }, 0xffffffff, { -1,  0,  0 } },// 21
	{ { -1, -1, +1 }, { 1, 1 }, 0xffffffff, { -1,  0,  0 } },// 22
	{ { -1, -1, -1 }, { 0, 1 }, 0xffffffff, { -1,  0,  0 } } // 23
};

static Shaders::Instanced::InputVertex0 iCube[24] =
{
	{ { -1, +1, +1 } },// 00
	{ { +1, +1, +1 } },// 01
	{ { +1, -1, +1 } },// 02
	{ { -1, -1, +1 } },// 03
	{ { -1, -1, +1 } },// 04
	{ { +1, -1, +1 } },// 05
	{ { +1, -1, -1 } },// 06
	{ { -1, -1, -1 } },// 07
	{ { -1, -1, -1 } },// 08
	{ { +1, -1, -1 } },// 09
	{ { +1, +1, -1 } },// 10
	{ { -1, +1, -1 } },// 11
	{ { -1, +1, -1 } },// 12
	{ { +1, +1, -1 } },// 13
	{ { +1, +1, +1 } },// 14
	{ { -1, +1, +1 } },// 15
	{ { +1, +1, +1 } },// 16
	{ { +1, +1, -1 } },// 17
	{ { +1, -1, -1 } },// 18
	{ { +1, -1, +1 } },// 19
	{ { -1, +1, -1 } },// 20
	{ { -1, +1, +1 } },// 21
	{ { -1, -1, +1 } },// 22
	{ { -1, -1, -1 } } // 23
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

int MyDXWindow::CreateGrid()
{
	vector<Shaders::Simple::InputVertex> v;
	v.reserve(201 * 4 + 2);
	for(int x = -100; x <= 100; x += 10)
	{
		float a = (float)x;
		float b = -100;
		float t = 100;
		if(x == 0)
		{
			t *= 10;
		}
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
	DXR(gridVB.Create(806, v.data(), StaticUsage));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

int MyDXWindow::CreateOctahedron()
{
	using namespace Shaders;

	float f = sqrtf(2) / 2.0f;
	Simple::InputVertex v[6] =
	{
		{ { 0, 0, 1 }, 0xffffffff },
		{ { -f, f, 0 }, 0xffffffff },
		{ { f, f, 0 }, 0xffffffff },
		{ { f, -f, 0 }, 0xffffffff },
		{ { -f, -f, 0 }, 0xffffffff },
		{ { 0, 0, -1 }, 0xffffffff }
	};
	DXR(octahedronVB.Create(6, v, StaticUsage));

	uint16 o[24] =
	{
		0, 1, 2, 0, 2, 3,
		0, 3, 4, 0, 4, 1,
		5, 2, 1, 5, 3, 2,
		5, 4, 3, 5, 1, 4
	};
	DXR(octahedronIB.Create(3 * 8, o, StaticUsage));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

const int fpsWidth = 256;
const int fpsHeight = 128;
const float fpsLeft = 200;
const float fpsTop = 200;
uint fpsScroll = 0;

bool MyDXWindow::OnCreate()
{
	TRACE("=== OnCreate() ===\n");

	if(!DXWindow::OnCreate())
	{
		return false;
	}

	DXB(physicsDebug.Create(this));

	Physics::Open();

	mGroundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
	btDefaultMotionState *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, mGroundShape, btVector3(0, 0, 0));
	mGroundRigidBody = new btRigidBody(groundRigidBodyCI);
	Physics::DynamicsWorld->addRigidBody(mGroundRigidBody, 1, -1);
	mGroundRigidBody->setRestitution(1);
	mGroundRigidBody->setFriction(1);

	btTransform bodyTransform(btQuaternion(btVector3(0.5f, 1, 0.25f), 1.5f), Vec4(0, 10, 20));
	Vec4f boxSize = Vec4(4, 4, 4);
	boxShape = new btBoxShape(boxSize);
	boxBody = new btRigidBody(100.0f, new btDefaultMotionState(bodyTransform), boxShape, Physics::inertia(100.0f, boxShape));
	boxBody->setFriction(0.1f);
	boxBody->setRestitution(0.25f);
	boxBody->setDamping(0, 0);
	Physics::DynamicsWorld->addRigidBody(boxBody);

	AssetManager::AddFolder("data");
	AssetManager::AddArchive("data.zip");

	FileBase *f;
	if(AssetManager::Open("cube.dae", &f) == S_OK)
	{
		f->Close();
	}

	// decompressor test
	if(false)
	{
		DiskFile d;
		if(d.Open(TEXT("Data/big.zip"), DiskFile::ForReading) == S_OK)
		{
			Archive a;
			if(a.Open(&d) == Archive::ok)
			{
				DiskFile d;
				if(d.Open("data/big.bin", DiskFile::ForReading) == S_OK)
				{
					Archive::File s;
					if(a.Locate("big.bin", s) == Archive::ok)
					{
						Ptr<byte> buffer(new byte[4096]);
						Ptr<byte> buffer2(new byte[4096]);
						size_t len;
						s.GetSize(len);
						size_t got;
						size_t total;
						while(s.Read(buffer2.get(), 4096, &got) == Archive::ok && len > 0)
						{
							len -= got;
							uint64 dgot;
							if(d.Read(buffer.get(), 4096, &dgot) == S_OK)
							{
								total += dgot;

								// check the results are the same
								int s = memcmp(buffer.get(), buffer2.get(), 4096);
								if(s != 0)
								{
									TRACE("Mismatch at %p!\n", total);
								}
							}
						}
					}
				}
			}
		}
	}

	DXB(scene.Load(TEXT("duck.dae")));

	DXB(FontManager::Open(this));

	DXB(debug_open(this));

	DXB(FontManager::Load(TEXT("debug"), &font));
	DXB(FontManager::Load(TEXT("Cooper_Black_48"), &bigFont));

	DXB(simpleShader.Create());
	DXB(simpleVB.Create(128, null, DynamicUsage, Writeable));

	DXB(CreateGrid());
	DXB(CreateOctahedron());

	DXB(cubeShader.Create());
	DXB(cubeTexture.Load(TEXT("temp.jpg")));
	Sampler::Options o;
	o.Filter = anisotropic;
	DXB(cubeSampler.Create(o));
	cubeShader.ps.picTexture = &cubeTexture;
	cubeShader.ps.tex1Sampler = &cubeSampler;
	DXB(cubeIndices.Create(_countof(indices), indices, StaticUsage));
	DXB(cubeVerts.Create(_countof(verts), verts, StaticUsage));

	DXB(instancedShader.Create());
	DXB(instancedVB0.Create(_countof(iCube), iCube, StaticUsage));
	DXB(instancedVB1.Create(64));

	lightPos = Vec4(0, -15, 20, 0);

	auto &l = cubeShader.ps.Light;
	l.lightPos = lightPos;
	l.ambientColor = Float3(0.3f, 0.3f, 0.3f);
	l.diffuseColor = Float3(0.7f, 0.7f, 0.7f);
	l.specColor = Float3(5, 5, 5);
	DXB(l.Update(Context()));

	DXB(uiShader.Create());
	DXB(UIVerts.Create(12, null, DynamicUsage, Writeable));
	DXB(uiTexture.Load(TEXT("temp.png")));
	DXB(uiSampler.Create());
	uiShader.ps.page = &uiTexture;
	uiShader.ps.smplr = &uiSampler;
	uiShader.vs.vConstants.TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
	DXB(uiShader.vs.vConstants.Update());
	uiShader.ps.pConstants.Color = Float4(1, 1, 1, 1);
	DXB(uiShader.ps.pConstants.Update());

	DXB(spriteShader.Create());
	DXB(spriteVerts.Create(2, null, DynamicUsage, Writeable));
	DXB(spriteTexture.Load(TEXT("temp.jpg")));
	DXB(spriteSampler.Create());
	spriteShader.ps.smplr = &spriteSampler;
	spriteShader.ps.page = &spriteTexture;

	DXB(spriteSheet.Load(TEXT("spriteSheet.xml")));

	DXB(renderTarget.Create(256, 256, RenderTarget::WithDepthBuffer));

	DXB(blitShader.Create());
	DXB(blitVB.Create(4, null, DynamicUsage, Writeable));

	blitShader.ps.smplr = &uiSampler;
	blitShader.ps.page = &renderTarget;

	cubeShader.ps.Camera.cameraPos = dashCam.position;
	DXB(cubeShader.ps.Camera.Update());

	dashCam.CalculatePerspectiveProjectionMatrix(0.5f, 1.0f);
	dashCam.position = Vec4(0, 0, 0);
	dashCam.LookAt(Vec4(0, 1, 0));
	dashCam.Update();

	DXB(splatShader.Create());
	DXB(splatVB.Create(8));

	DXB(fpsGraph.Create(fpsWidth, fpsHeight, RenderTarget::WithoutDepthBuffer));
	DXB(fpsSampler.Create(Sampler::Options(TextureFilter::min_mag_mip_point, TextureAddressWrap, TextureAddressWrap)));

	TRACE("=== End of OnCreate() ===\n");

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnFrame()
{
	oldDeltaTime = deltaTime;
	deltaTime = (float)mTimer.Delta();
	float time = (float)mTimer.WallTime();

	debug_begin();

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

	camera.CalculatePerspectiveProjectionMatrix(0.5f, (float)ClientWidth() / ClientHeight());

	auto &vc = blitShader.vs.vConstants;
	vc.TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
	vc.Update(Context());

	camera.Move(move);
	camera.Update();

	if(Keyboard::Pressed('B'))
	{
		boxBody->activate(true);
		boxBody->applyCentralImpulse(btVector3(0, 20, 3000));
	}

	Physics::DynamicsWorld->stepSimulation(deltaTime * 2, 20, (deltaTime * 2) / 20);

	cubePos = Vec4(15, 15, 0);
	cubeScale = Vec4(5, 5, 5);
	cubeRot = Vec4(time * 0.8f, time * 0.9f, time * 0.7f);

	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	// Draw spinning cube

	{
		Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(cubePos);

		auto &vc = cubeShader.vs.VertConstants;
		vc.TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
		vc.ModelMatrix = Transpose(modelMatrix);
		vc.Update(Context());

		auto &ps = cubeShader.ps.Camera;
		ps.cameraPos = camera.position;
		ps.Update(Context());

		cubeShader.Activate(Context());
		cubeShader.vs.SetVertexBuffers(Context(), 1, &cubeVerts);
		cubeIndices.Activate(Context());

		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexed(cubeIndices.Count(), 0, 0);
	}

	// Draw loaded model

	Matrix bob = ScaleMatrix(Vec4(0.1f, 0.1f, 0.1f)) * TranslationMatrix(Vec4(0, -5, 0)) * RotationMatrix(time * 1.0f, time * 1.2f, time * 1.3f);
	scene.Render(Context(), bob, camera.GetTransformMatrix(bob), camera.position);

	// Instanced cubes

	{
		Random r(0);
		uint c = instancedVB1.Count();
		Shaders::Instanced::InputVertex1 *p;
		instancedVB1.Map(Context(), p);
		for(uint i = 0; i < c; ++i)
		{
			float t = i * PI * 2 / c + time / 10;
			float x = sinf(t) * 50;
			float y = cosf(t) * 50;
			*p++ = { { x, y, 0 }, r.Next() };
		}
		instancedVB1.UnMap(Context());

		instancedShader.vs.VertConstants.Get()->Transform = Transpose(camera.GetTransformMatrix());
		instancedShader.Activate(Context());
		instancedShader.vs.SetVertexBuffers(Context(), 2, &instancedVB0, &instancedVB1);
		cubeIndices.Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context()->DrawIndexedInstanced(cubeIndices.Count(), c, 0, 0, 0);
	}

	// Draw grid

	simpleShader.Activate(Context());
	simpleShader.vs.VertConstants.Get()->TransformMatrix = Transpose(camera.GetTransformMatrix());
	simpleShader.vs.SetVertexBuffers(Context(), 1, &gridVB);
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(gridVB.Count(), 0);

	// Draw light

	Matrix modelMatrix = ScaleMatrix(Vec4(0.25f, 0.25f, 0.25f));
	modelMatrix *= TranslationMatrix(lightPos);
	simpleShader.vs.VertConstants.Get()->TransformMatrix = Transpose(camera.GetTransformMatrix(modelMatrix));
	simpleShader.vs.SetVertexBuffers(Context(), 1, &octahedronVB);
	octahedronIB.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(octahedronIB.Count(), 0, 0);

	// Drawlist UI elements

	{
		using vrt = Shaders::UI::InputVertex;
		drawList.Reset(Context(), &uiShader, &UIVerts);
		uiShader.ps.page = &uiTexture;
		uiShader.ps.smplr = &uiSampler;

		float w = 100 + sinf(time * 0.9f) * 30;
		float h = 200 + sinf(time * 0.9f) * 30;
		float x0 = 0;
		float y0 = 0;
		float x1 = x0 + w;
		float y1 = y0 + h;

		Shaders::UI::PS::pConstants_t c;
		Shaders::UI::VS::vConstants_t v;
		v.TransformMatrix = Transpose(OrthoProjection2D(1920, 1080));
		c.Color = Float4(1, 1, 1, sinf(time * 10) * 0.5f + 0.5f);
		drawList.SetConstantData(Vertex, v, Shaders::UI::VS::vConstants_index);
		drawList.SetConstantData(Pixel, c, Shaders::UI::PS::pConstants_index);
		drawList.BeginTriangleList();
		drawList.AddVertex<vrt>({ { x0, y0, }, { 0, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y0 }, { 1, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x0, y1 }, { 0, 1 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y0, }, { 1, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y1 }, { 1, 1 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x0, y1 }, { 0, 1 }, 0xffffffff });
		drawList.End();

		w = 100 + sinf(time * 1.1f) * 30;
		h = 200 + sinf(time * 1.2f) * 30;
		x0 += 200;
		y0 += 200;
		x1 = x0 + w;
		y1 = y0 + h;

		c.Color = Float4(1, 1, 1, 1);
		drawList.SetConstantData(Pixel, c, uiShader.ps.pConstants.Index());
		drawList.BeginTriangleList();
		drawList.AddVertex<vrt>({ { x0, y0, }, { 0, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y0 }, { 1, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x0, y1 }, { 0, 1 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y0, }, { 1, 0 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x1, y1 }, { 1, 1 }, 0xffffffff });
		drawList.AddVertex<vrt>({ { x0, y1 }, { 0, 1 }, 0xffffffff });
		drawList.End();
	}

	// Drawlist simple rectangle
		
	{
		using vrt = Shaders::Simple::InputVertex;
		drawList.Reset(Context(), &simpleShader, &simpleVB);

		Shaders::Simple::VS::VertConstants_t v;
		v.TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
		drawList.SetConstantData(Vertex, v, 0);
		drawList.BeginTriangleStrip();
		drawList.AddVertex<vrt>({ { 0, 0, 0.5f }, 0x80000000 });
		drawList.AddVertex<vrt>({ { FClientWidth(), 0, 0.5f }, 0x80000000 });
		drawList.AddVertex<vrt>({ { 0, 100, 0.5f }, 0x80000000 });
		drawList.AddVertex<vrt>({ { FClientWidth(), 100, 0.5f }, 0x80000000 });
		drawList.End();
	}

	// Drawlist some text

	debug_text("DeltaTime % 8.2fms (% 3dfps)\n", deltaTime * 1000, (int)(1 / deltaTime));
	debug_text("Yaw: %4d, Pitch: %4d, Roll: %4d\n", (int)Rad2Deg(camera.yaw), (int)Rad2Deg(camera.pitch), (int)Rad2Deg(camera.roll));

	bigFont->SetDrawList(drawList);
	bigFont->Setup(Context(), this);
	bigFont->Begin();
	bigFont->DrawString("HELLO WORLD", Vec2f(FClientWidth() / 2, FClientHeight()), Font::HCentre, Font::VBottom);
	bigFont->End();

	drawList.Execute();

	// Draw some sprites immediate mode

	spriteShader.gs.vConstants.Get()->TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));

	Sprite *v;
	spriteVerts.Map(Context(), (Shaders::Sprite::InputVertex * &)v);

	v[0].Position = { ClientWidth() - 100.0f, ClientHeight() - 100.0f };
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

	spriteVerts.UnMap(Context());
	spriteShader.vs.SetVertexBuffers(Context(), 1, &spriteVerts);
	spriteShader.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	Context()->Draw(spriteVerts.Count(), 0);

	{
		// Drawlist some sprites

		Sprite const &s = spriteSheet["temp.png"];
		Sprite const &t = spriteSheet["temp.jpg"];
		spriteSheet.SetupTransform(Context(), ClientWidth(), ClientHeight());

		drawList.Reset(Context(), &spriteSheet.mShader, &spriteSheet.mVertexBuffer);
		drawList.BeginPointList();
		Sprite &q = drawList.AddVertex<Sprite>();
		q.Set(t, { 100, ClientHeight() - 100.0f });
		q.Rotation = time * PI / 2;
		drawList.End();
		drawList.Execute();

		// Draw some SpriteSheet sprites

		bool xflip = ((int)(time * 10) & 1) != 0;
		bool yflip = ((int)(time * 5) & 1) != 0;

		spriteSheet.BeginRun(Context());
		spriteSheet.Add(s,
						 { sinf(time * 2) * 200 + 200, cosf(time * 1.5f) * 200 + 200 },
						 { sinf(time * 1.75f) * 0.1f + 0.2f, cosf(time * 1.9f) * 0.1f + 0.2f },
						 { 0.5f, 0.5f },
						 0,
						 Color::White,
						 xflip, yflip);
		spriteSheet.ExecuteRun(Context());
	}

	// Draw spinning cube into rendertarget

	{
		Matrix modelMatrix = RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(Vec4(0, 30, 0));
		auto &vc = cubeShader.vs.VertConstants;
		vc.TransformMatrix = Transpose(dashCam.GetTransformMatrix(modelMatrix));
		vc.ModelMatrix = Transpose(modelMatrix);
		vc.Update();
	}

	renderTarget.Activate(Context());
	renderTarget.Clear(Context(), 0x208040);
	renderTarget.ClearDepth(Context(), DepthOnly);
	Viewport(0, 0, renderTarget.FWidth(), renderTarget.FHeight(), 0, 1).Activate(Context());
	cubeShader.Activate(Context());
	cubeShader.vs.SetVertexBuffers(Context(), 1, &cubeVerts);
	cubeIndices.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cubeIndices.Count(), 0, 0);

	ResetRenderTargetView();

	// Blit the rendertarget onto backbuffer

	float w = renderTarget.FWidth();
	float h = renderTarget.FHeight();
	float r = (float)ClientWidth() - 10;
	float l = r - w;
	float t = 10;
	float b = t + h;
	Shaders::Blit::InputVertex *bv;
	blitVB.Map(Context(), bv);
	bv[0] = { { l, t }, { 0, 0 } };
	bv[1] = { { r, t }, { 1, 0 } };
	bv[2] = { { l, b }, { 0, 1 } };
	bv[3] = { { r, b }, { 1, 1 } };
	blitVB.UnMap(Context());

	blitShader.ps.page = &renderTarget;
	blitShader.ps.smplr = &uiSampler;
	blitShader.Activate(Context());
	blitShader.vs.SetVertexBuffers(Context(), 1, &blitVB);
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Context()->Draw(blitVB.Count(), 0);

	// Draw graph

	{
		int speed = 4;
		float x0 = (float)fpsScroll;
		float x1 = (float)fpsScroll + speed;
		fpsScroll = (fpsScroll + speed) % fpsWidth;
		float h = (float)fpsHeight;

		Viewport(0, 0, fpsGraph.FWidth(), fpsGraph.FHeight(), 0, 1).Activate(Context());
		fpsGraph.Activate(Context());
		splatShader.vs.vConstants.Get()->TransformMatrix = Transpose(OrthoProjection2D(fpsGraph.Width(), fpsGraph.Height()));
		splatShader.vs.SetVertexBuffers(Context(), 1, &splatVB);
		splatShader.Activate(Context());
		
		Shaders::Splat::InputVertex *p;
		splatVB.Map(Context(), p);
		*p++ = { { x0, 0 }, 0xc0000000 };
		*p++ = { { x1, 0 }, 0xc0000000 };
		*p++ = { { x0, h }, 0xc0000000 };
		*p++ = { { x1, h }, 0xc0000000 };
		splatVB.UnMap(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Context()->Draw(4, 0);

		float y0 = fpsHeight - oldDeltaTime * 32 * fpsHeight;
		float y1 = fpsHeight - deltaTime * 32 * fpsHeight;

		splatVB.Map(Context(), p);
		*p++ = { { x0, y0 }, 0xffffffff };
		*p++ = { { x1, y1 }, 0xffffffff };
		splatVB.UnMap(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		Context()->Draw(2, 0);

		ResetRenderTargetView();

		float u0 = fpsScroll / (float)fpsWidth;
		float u1 = u0 + 1;
		Shaders::UI::InputVertex *v;
		UIVerts.Map(Context(), v);
		v[0] = { { fpsLeft, fpsTop }, { u0, 0 }, 0xffffffff };
		v[1] = { { fpsLeft + fpsWidth, fpsTop }, { u1, 0 }, 0xffffffff };
		v[2] = { { fpsLeft, fpsTop + fpsHeight }, { u0, 1 }, 0xffffffff };
		v[3] = { { fpsLeft + fpsWidth, fpsTop + fpsHeight }, { u1, 1 }, 0xffffffff };
		UIVerts.UnMap(Context());

		uiShader.vs.SetVertexBuffers(Context(), 1, &UIVerts);
		uiShader.vs.vConstants.Get()->TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
		uiShader.ps.pConstants.Get()->Color = Vec4(1, 1, 1, 1);
		uiShader.ps.page = &fpsGraph;
		uiShader.ps.smplr = &fpsSampler;
		uiShader.Activate(Context());
		Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Context()->Draw(4, 0);
	}

	Physics::DynamicsWorld->setDebugDrawer(&physicsDebug);
	physicsDebug.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	physicsDebug.BeginScene(&camera);
	Physics::DynamicsWorld->debugDrawWorld();
	physicsDebug.EndScene();

	debug_end();
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	TRACE("=== BEGINNING OF OnDestroy() ===\n");

	Physics::Close();
	physicsDebug.Release();

	debug_close();

	cubeShader.Release();
	cubeVerts.Release();
	cubeIndices.Release();
	cubeTexture.Release();
	cubeSampler.Release();
	simpleShader.Release();
	gridVB.Release();
	octahedronVB.Release();
	simpleVB.Release();
	octahedronIB.Release();
	instancedShader.Release();
	instancedVB0.Release();
	instancedVB1.Release();
	scene.Unload();
	uiShader.Release();
	UIVerts.Release();
	uiTexture.Release();
	uiSampler.Release();

	font.Release();
	bigFont.Release();

	spriteShader.Release();
	spriteVerts.Release();
	spriteTexture.Release();
	spriteSampler.Release();

	splatShader.Release();
	splatVB.Release();

	fpsGraph.Release();
	fpsSampler.Release();

	spriteSheet.Release();

	renderTarget.Release();

	blitShader.Release();
	blitVB.Release();

	Scene::CleanUp();
	Texture::FlushAll();
	FontManager::Close();

	AssetManager::Close();

	TRACE("=== END OF OnDestroy() ===\n");
}
