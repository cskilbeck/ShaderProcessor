//////////////////////////////////////////////////////////////////////

#pragma once

using namespace DX;

#include "Phong.h"
#include "Colored.h"

//////////////////////////////////////////////////////////////////////

struct FPSCamera: Camera
{
	Vec4f position;
	float yaw;
	float pitch;
	float roll;

	FPSCamera()
		: position(Vec4(0,0,10))
		, yaw(0)
		, pitch(-PI)
		, roll(0)
	{
		CalculatePerspectiveProjectionMatrix();
		Update();
	}

	void LookAt(Vec4f target)
	{
		CalculateViewMatrix(target, position, Vec4(0, 0, 1));
		Vec4f pyr = GetPitchYawRollFromMatrix(GetViewMatrix());
		pitch = GetX(pyr);
		yaw = GetY(pyr);
		roll = GetZ(pyr);
	}

	void Move(Vec4f direction)
	{
		position += GetStrafeVector() * GetX(direction);
		position += GetFlatForwardVector() * GetY(direction);
		position += Vec4(0, 0, GetZ(direction));
	}

	void Rotate(float pan, float tilt)
	{
		pitch = Constrain(pitch - tilt, -PI + FLT_EPSILON, FLT_EPSILON);
		yaw -= pan;
	}

	void Update()
	{
		CalculateViewMatrix(position, roll, pitch, yaw);
		CalculateViewProjectionMatrix();
	}
};

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

