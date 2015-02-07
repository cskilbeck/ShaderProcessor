//////////////////////////////////////////////////////////////////////

#pragma once

#include "phong.h"
#include "ColoredLine.h"

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
		CalculateViewProjectionMatrix();
		CalculateViewMatrix(position, roll, pitch, yaw);
	}

	void Reset()
	{
		position = Vec4(0, 0, -10);
		yaw = pitch = roll = 0;
		CalculatePerspectiveProjectionMatrix();
		CalculateViewProjectionMatrix();
		CalculateViewMatrix(position, roll, pitch, yaw);
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
	Matrix worldMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	Ptr<vs_phong> vs;
	Ptr<ps_phong> ps;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;
	Ptr<IndexBuffer<uint16>> indexBuffer;
	Ptr<vs_phong::VertexBuffer> vertexBuffer;
	DXPtr<ID3D11RasterizerState> rasterizerState;
	DXPtr<ID3D11DepthStencilState>	depthStencilState;
	DXPtr<ID3D11BlendState>	blendState;

	Ptr<vs_ColoredLine> vsLine;
	Ptr<ps_ColoredLine> psLine;
	Ptr<vs_ColoredLine::VertexBuffer> vbLineGrid;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;
};

