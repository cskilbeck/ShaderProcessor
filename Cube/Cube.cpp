//////////////////////////////////////////////////////////////////////
// RTCB001: don't return a bool to indicate success or failure (return a signed integer, where a negative value indicates failure)
// RTCB002: don't do anything that can fail in a constructor (unless you dig exceptions, in which case throw up all you like)
// RTCB003: when declaring function parameters, prefer enum to bool
// RTCB004: 

// Transparency
// Shared constant buffers:
//		if name begins with g_
//		and both names are the same
//		and both bindpoints are the same
//			and map/unmap hasn't been called on the buffer
//			then don't re-set it
// Physics: fixed timesteps for deterministic behaviour (enable vblank and use framecount)
// Track and fix leaks
// Cartoon Car Physics
// Normal mapping
// Shadow mapping
// Track resources and clean them up automatically (with warnings, like Font does)
//		Textures
//		Samplers
//		etc
// 2D UI Elements/SceneGraph
// zLib: support Deflate64
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
// * Bullet
//		* Test the AssetManager

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

//////////////////////////////////////////////////////////////////////

namespace
{
	float dy1 = 0.0f;
	float dy2 = 1.0f/3;
	float dy3 = 2.0f/3;
	float dy4 = 1.0f;

	float dx1 = 0.0f;
	float dx2 = 0.5f;
	float dx3 = 1.0f;
}

static Shaders::Phong::InputVertex diceVerts[24] =
{
	// TOP 1

	{ { -1, +1, +1 }, { dx1, dy1 }, Color::BrightRed, { 0, 0, +1 } },// 00
	{ { +1, +1, +1 }, { dx2, dy1 }, Color::BrightRed, { 0, 0, +1 } },// 01
	{ { +1, -1, +1 }, { dx2, dy2 }, Color::BrightRed, { 0, 0, +1 } },// 02
	{ { -1, -1, +1 }, { dx1, dy2 }, Color::BrightRed, { 0, 0, +1 } },// 03

	// BACK 2
						    
	{ { -1, -1, +1 }, { dx1, dy2 }, Color::BrightGreen, { 0, -1, 0 } },// 04
	{ { +1, -1, +1 }, { dx2, dy2 }, Color::BrightGreen, { 0, -1, 0 } },// 05
	{ { +1, -1, -1 }, { dx2, dy3 }, Color::BrightGreen, { 0, -1, 0 } },// 06
	{ { -1, -1, -1 }, { dx1, dy3 }, Color::BrightGreen, { 0, -1, 0 } },// 07

	// BOTTOM 6

	{ { -1, -1, -1 }, { dx2, dy3 }, Color::BrightBlue, { 0, 0, -1 } },// 08
	{ { +1, -1, -1 }, { dx3, dy3 }, Color::BrightBlue, { 0, 0, -1 } },// 09
	{ { +1, +1, -1 }, { dx3, dy4 }, Color::BrightBlue, { 0, 0, -1 } },// 10
	{ { -1, +1, -1 }, { dx2, dy4 }, Color::BrightBlue, { 0, 0, -1 } },// 11

	// FRONT 5
						    
	{ { -1, +1, -1 }, { dx2, dy2 }, Color::Cyan, { 0, +1, 0 } },// 12
	{ { +1, +1, -1 }, { dx3, dy2 }, Color::Cyan, { 0, +1, 0 } },// 13
	{ { +1, +1, +1 }, { dx3, dy3 }, Color::Cyan, { 0, +1, 0 } },// 14
	{ { -1, +1, +1 }, { dx2, dy3 }, Color::Cyan, { 0, +1, 0 } },// 15

	// RIGHT 3
						    
	{ { +1, +1, +1 }, { dx1, dy3 }, Color::Magenta, { +1, 0, 0 } },// 16
	{ { +1, +1, -1 }, { dx2, dy3 }, Color::Magenta, { +1, 0, 0 } },// 17
	{ { +1, -1, -1 }, { dx2, dy4 }, Color::Magenta, { +1, 0, 0 } },// 18
	{ { +1, -1, +1 }, { dx1, dy4 }, Color::Magenta, { +1, 0, 0 } },// 19

	// LEFT 4
						    
	{ { -1, +1, -1 }, { dx2, dy1 }, Color::Yellow, { -1, 0, 0 } },// 20
	{ { -1, +1, +1 }, { dx3, dy1 }, Color::Yellow, { -1, 0, 0 } },// 21
	{ { -1, -1, +1 }, { dx3, dy2 }, Color::Yellow, { -1, 0, 0 } },// 22
	{ { -1, -1, -1 }, { dx2, dy2 }, Color::Yellow, { -1, 0, 0 } } // 23
};

//////////////////////////////////////////////////////////////////////

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

MyDXWindow::Box MyDXWindow::box[MyDXWindow::numBoxes];

//////////////////////////////////////////////////////////////////////

void MyDXWindow::Box::Create(Vec4f pos)
{
	Destroy();
//	btTransform bodyTransform(btQuaternion(btVector3(0.5f, 1, 0.25f), 1.5f), pos);
	btTransform bodyTransform(btQuaternion::getIdentity(), pos);
	Vec4f boxSize = Vec4(4, 4, 4);
	mShape = new btBoxShape(boxSize);
	mBody = Physics::CreateRigidBody(500.0f, bodyTransform, mShape);
	mBody->setFriction(0.5);
	mBody->setRestitution(0.0f);
	mBody->setDamping(0.01f, 0.1f);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::Box::Destroy()
{
	if(mBody != null)
	{
		Physics::DynamicsWorld->removeRigidBody(mBody);
		Delete(mBody);
	}
	Delete(mShape);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::Box::Draw(MyDXWindow *window)
{
	Matrix modelMatrix = ScaleMatrix(Vec4(4, 4, 4)) * Physics::btTransformToMatrix(mBody->getWorldTransform());
	window->DrawCube(modelMatrix, window->diceVertBuffer, window->diceTexture);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::DrawCube(Matrix const &m, VertexBuffer<Shaders::Phong::InputVertex> &cubeVerts, Texture &texture)
{
	auto &vc = cubeShader.vs.VertConstants;
	vc.TransformMatrix = Transpose(camera.GetTransformMatrix(m));
	vc.ModelMatrix = Transpose(m);
	vc.Update(Context());
	auto &ps = cubeShader.ps.Camera;
	ps.cameraPos = camera.position;
	ps.Update(Context());
	cubeShader.ps.picTexture = &texture;
	cubeShader.Activate(Context());
	cubeShader.vs.SetVertexBuffers(Context(), 1, &cubeVerts);
	cubeIndices.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cubeIndices.Count(), 0, 0);
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::SetupBoxes()
{
	for(int i = 0; i < numBoxes; ++i)
	{
		box[i].Create(Vec4(0, 50, (float)i * 8 + 4));
	}
}

int MyDXWindow::CreateSphere(int steps)
{
	float a = 1 / sqrtf(2);
	Vec4f p[] = 
	{
		{ -a, a, 0 }, { a, a, 0 }, { -a, -a, 0 }, { a, -a, 0 },
		{ 0, -a, a }, { 0, a, a }, { 0, -a, -a }, { 0, a, -a },
		{ a, 0, -a }, { a, 0, a }, { -a, 0, -a }, { -a, 0, a }
	};

	struct Tri
	{
		Vec4f p1, p2, p3;
		Tri() { }
		Tri(Vec4f a, Vec4f b, Vec4f c) : p1(a) , p2(b) , p3(c) { }
	};

	int vsize = IntPow(4, steps) * 20;
	vector<Tri> f(vsize);

	f[0] = Tri(p[0], p[11], p[5]);
	f[1] = Tri(p[0], p[5], p[1]);
	f[2] = Tri(p[0], p[1], p[7]);
	f[3] = Tri(p[0], p[7], p[10]);
	f[4] = Tri(p[0], p[10], p[11]);

	f[5] = Tri(p[1], p[5], p[9]);
	f[6] = Tri(p[5], p[11], p[4]);
	f[7] = Tri(p[11], p[10], p[2]);
	f[8] = Tri(p[10], p[7], p[6]);
	f[9] = Tri(p[7], p[1], p[8]);

	f[10] = Tri(p[3], p[9], p[4]);
	f[11] = Tri(p[3], p[4], p[2]);
	f[12] = Tri(p[3], p[2], p[6]);
	f[13] = Tri(p[3], p[6], p[8]);
	f[14] = Tri(p[3], p[8], p[9]);

	f[15] = Tri(p[4], p[9], p[5]);
	f[16] = Tri(p[2], p[4], p[11]);
	f[17] = Tri(p[6], p[2], p[10]);
	f[18] = Tri(p[8], p[6], p[7]);
	f[19] = Tri(p[9], p[8], p[1]);

	int nt = 20;

	for(int it = 0; it < steps; it++)
	{
		int ntold = nt;
		for(int i = 0; i < ntold; i++)
		{
			Vec4f pa = Normalize(Average(f[i].p1, f[i].p2));
			Vec4f pb = Normalize(Average(f[i].p2, f[i].p3));
			Vec4f pc = Normalize(Average(f[i].p3, f[i].p1));
			f[nt++] = Tri(f[i].p1, pa, pc);
			f[nt++] = Tri(pa, f[i].p2, pb);
			f[nt++] = Tri(pb, f[i].p3, pc);
			f[i] = Tri(pa, pb, pc);
		}
	}

	vector<Shaders::Sphere::InputVertex> v(vsize * 3);
	uint n = 0;
	for(int i = 0; i < vsize; ++i)
	{
		v[n++] = { f[i].p1 };
		v[n++] = { f[i].p3 };
		v[n++] = { f[i].p2 };
	}
	DXR(sphereVerts.Create(n, v.data(), StaticUsage, NotCPUAccessible));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::DrawSphere(Matrix const &m, Texture &texture)
{
	auto &vc = sphereShader.vs.VertConstants;
	vc.TransformMatrix = Transpose(camera.GetTransformMatrix(m));
	vc.ModelMatrix = Transpose(m);
	vc.Update(Context());
	auto &ps = sphereShader.ps.Camera;
	ps.cameraPos = camera.position;
	auto &l = sphereShader.ps.Light;
	l.lightPos = lightPos;
	l.ambientColor = Float3(0.1f, 0.1f, 0.1f);
	l.diffuseColor = Float3(1.7f, 1.7f, 1.7f);
	l.specColor = Float3(2, 2, 2);
	l.Update(Context());
	ps.Update(Context());
	sphereShader.ps.sphereTexture = &sphereTexture;
	sphereShader.ps.tex1Sampler = &cubeSampler;
	sphereShader.vs.SetVertexBuffers(Context(), 1, &sphereVerts);
	sphereShader.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->Draw(sphereVerts.Count(), 0);
}

//////////////////////////////////////////////////////////////////////

int MyDXWindow::CreateCylinder(int steps)
{
	vector<Shaders::Phong::InputVertex> v;
	v.resize((steps + 1) * 2 + steps * 2);

	int tb = (steps + 1) * 2;
	int bb = tb + steps;
	for(int i = 0; i <= steps; ++i)
	{
		half u = i / (float)steps;
		float s, c;
		float t = i * (PI * 2 / steps);
		DirectX::XMScalarSinCos(&s, &c, t);
		float s2 = s / 2;
		float c2 = c / 2;
		v[i * 2] = { Float3(s2, c2, 0.5f), Half2(u, 1), 0xffffffff, Float3(s, c, 0) };
		v[i * 2 + 1] = { Float3(s2, c2, -0.5f), Half2(u, 0), 0xffffffff, Float3(s, c, 0) };
		if(i < steps)
		{
			v[tb + i] = { Float3(s2, c2, 0.5f), Half2(s2 + 0.5f, c2 + 0.5f), 0xffffffff, Float3(0, 0, 1) };
			v[bb + i] = { Float3(s2, c2, -0.5f), Half2(s2 + 0.5f, c2 + 0.5f), 0xffffffff, Float3(0, 0, -1) };
		}
	}
	DXR(cylinderVerts.Create((uint)v.size(), v.data(), StaticUsage, NotCPUAccessible));

	vector<uint16> idx;
	idx.reserve(steps * 12);

	// barrel
	for(uint16 i = 0; i < steps; i++)
	{
		uint16 t = i * 2;
		idx.push_back(t);
		idx.push_back(t+1);
		idx.push_back(t+2);
		idx.push_back(t+1);
		idx.push_back(t+3);
		idx.push_back(t+2);
	}

	// top
	for(int i = 0; i < steps - 1; ++i)
	{
		idx.push_back(tb);
		idx.push_back(tb + i);
		idx.push_back(tb + i + 1);
	}

	// bottom
	for(int i = 0; i < steps - 1; ++i)
	{
		idx.push_back(bb);
		idx.push_back(bb + i + 1);
		idx.push_back(bb + i);
	}
	DXR(cylinderIndices.Create((uint16)idx.size(), idx.data(), StaticUsage, NotCPUAccessible));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::DrawCylinder(Matrix const &m, Texture &texture)
{
	auto &vc = cubeShader.vs.VertConstants;
	vc.TransformMatrix = Transpose(camera.GetTransformMatrix(m));
	vc.ModelMatrix = Transpose(m);
	vc.Update(Context());
	auto &ps = cubeShader.ps.Camera;
	ps.cameraPos = camera.position;
	ps.Update(Context());
	cubeShader.ps.picTexture = &texture;
	cubeShader.vs.SetVertexBuffers(Context(), 1, &cylinderVerts);
	cubeShader.Activate(Context());
	cylinderIndices.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cylinderIndices.Count(), 0, 0);
}

//////////////////////////////////////////////////////////////////////

int MyDXWindow::CreateGrid()
{
	const int size = 100;
	const int gap = 10;
	vector<Shaders::Simple::InputVertex> v;
	for(int x = -size; x <= size; x += gap)
	{
		float a = (float)x;
		float b = -size;
		float t = size;
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
	v.push_back({ { 0, 0, size * 10 }, Color::Cyan });
	DXR(gridVB.Create((uint)v.size(), v.data(), StaticUsage));
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

//////////////////////////////////////////////////////////////////////

bool MyDXWindow::OnCreate()
{
	TRACE("=== OnCreate() ===\n");

	if(!DXWindow::OnCreate())
	{
		return false;
	}

	debugPhysics = true;
	Physics::Open(this);

	DXB(car.Create(Vec4(0, 0, 1.45f)));

	mGroundShape = new btBoxShape(btVector3(100, 100, 1));
	btDefaultMotionState *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -1)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, mGroundShape, btVector3(0, 0, 0));
	mGroundRigidBody = new btRigidBody(groundRigidBodyCI);
	Physics::DynamicsWorld->addRigidBody(mGroundRigidBody, 1, -1);
	mGroundRigidBody->setRestitution(1);
	mGroundRigidBody->setFriction(1);

	// Create some boxes
	SetupBoxes();

	AssetManager::AddFolder("Data");
	AssetManager::AddArchive("data.zip");

	// decompressor test
	if(false)
	{
		DiskFile d;
		if(d.Open(TEXT("big.zip"), DiskFile::ForReading) == S_OK)
		{
			Archive a;
			if(a.Open(&d) == Archive::ok)
			{
				DiskFile d;
				if(d.Open("big.bin", DiskFile::ForReading) == S_OK)
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

	camera.Load();

	DXB(scene.Load(TEXT("duck.dae")));

	DXB(FontManager::Open(this));

	DXB(debug_open(this));

	DXB(CreateCylinder(32));
	DXB(CreateSphere(4));

	DXB(FontManager::Load(TEXT("debug"), &font));
	DXB(FontManager::Load(TEXT("Cooper_Black_48"), &bigFont));

	DXB(simpleShader.Create());
	DXB(simpleVB.Create(128, null, DynamicUsage, Writeable));

	DXB(CreateGrid());
	DXB(CreateOctahedron());

	DXB(sphereShader.Create());

	DXB(cubeShader.Create());
	DXB(cubeTexture.Load(TEXT("temp.jpg")));
	DXB(diceTexture.Load(TEXT("dice.png")));
	DXB(sphereTexture.Load(TEXT("earth.dds")));
	Sampler::Options o;
	o.Filter = anisotropic;
	DXB(cubeSampler.Create(o));
	cubeShader.ps.picTexture = &cubeTexture;
	cubeShader.ps.tex1Sampler = &cubeSampler;
	DXB(cubeIndices.Create(_countof(indices), indices, StaticUsage));
	DXB(cubeVerts.Create(_countof(verts), verts, StaticUsage));
	DXB(diceVertBuffer.Create(_countof(diceVerts), diceVerts, StaticUsage));

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

	float aspect = (float)ClientWidth() / ClientHeight();
	camera.CalculatePerspectiveProjectionMatrix(0.5f, aspect);

	car.Update(deltaTime);

	auto &vc = blitShader.vs.vConstants;
	vc.TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
	vc.Update(Context());

	camera.Move(move);
	camera.Update();

	if(Keyboard::Pressed('B'))
	{
		SetupBoxes();
		car.Reset();
	}

	if(Keyboard::Pressed('N'))
	{
		Random r;
		for(int i = 0; i < numBoxes; ++i)
		{
			box[i].mBody->activate(true);
			box[i].mBody->applyCentralImpulse(btVector3(0, 0, r.Ranged(20000) + 10000.0f));
			box[i].mBody->applyTorqueImpulse(btVector3(r.Ranged(10000) + 15000.0f, r.Ranged(10000) + 15000.0f, r.Ranged(10000) + 5000.0f));
		}
	}

	Physics::DynamicsWorld->stepSimulation(deltaTime * 4, 20, 1 / 120.0f);

	cubePos = Vec4(15, 15, 0);
	cubeScale = Vec4(5, 5, 5);
	cubeRot = Vec4(time * 0.8f, time * 0.9f, time * 0.7f);

	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	for(int i = 0; i < numBoxes; ++i)
	{
		box[i].Draw(this);
	}

	if(!debugPhysics)
	{
		car.Draw(this);
	}

	DrawCube(RotationMatrix(cubeRot) * ScaleMatrix(cubeScale) * TranslationMatrix(cubePos), cubeVerts, cubeTexture);

	DrawCylinder(RotationMatrix(Vec4(time, time * 0.3f, time * 0.27f)) * ScaleMatrix(Vec4(10, 10, 10)) * TranslationMatrix(Vec4(-20, 20, 0)), cubeTexture);

	DrawSphere(RotationMatrix(Vec4(time * 0.5f, PI / 2, PI / 2)) * ScaleMatrix(Vec4(10, 10, 10)) * TranslationMatrix(Vec4(-20, -20, 0)), cubeTexture);

	Matrix bob = RotationMatrix(time * 1.0f, time * 1.2f, time * 1.3f) * ScaleMatrix(Vec4(0.1f, 0.1f, 0.1f)) * TranslationMatrix(Vec4(20, -20, 0));
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
	cubeShader.ps.picTexture = &cubeTexture;
	cubeShader.vs.SetVertexBuffers(Context(), 1, &cubeVerts);
	cubeShader.Activate(Context());
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

	if(Keyboard::Pressed(VK_RETURN))
	{
		debugPhysics = !debugPhysics;
	}

	if(debugPhysics)
	{
		Physics::DebugBegin(&camera);
		Physics::DynamicsWorld->debugDrawWorld();
		Physics::DebugEnd();
	}

	debug_end();
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	TRACE("=== BEGINNING OF OnDestroy() ===\n");

	camera.Save();

	Physics::Close();

	debug_close();

	diceTexture.Release();
	sphereTexture.Release();

	cylinderIndices.Release();
	cylinderVerts.Release();
	sphereVerts.Release();

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
