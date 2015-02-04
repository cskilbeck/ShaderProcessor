//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct FPSCamera: Camera
{
	Vec4f position;
	float yaw;
	float pitch;

	FPSCamera()
		: position(Vec4(0,0,0))
		, yaw(0)
		, pitch(0)
	{
	}

	void Update()
	{
		CalculateViewMatrix(position, yaw, pitch, 0);
		CalculateViewProjectionMatrix();
	}

	void LookAt(Vec4f target)
	{
		CalculateViewMatrix(target, position, Vec4(0, 0, 1));
		Vec4f o = GetYawPitchRollFromMatrix(GetViewMatrix());
		yaw = GetX(o);
		pitch = GetY(o);
	}

	void Move(Vec4f direction)
	{
		position += GetStrafeVector() * GetX(direction);
		position += GetForwardVector() * GetZ(direction);
		position += GetUpVector() * GetY(direction);
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
	Ptr<vs_shader> vertexShader;
	Ptr<ps_shader> pixelShader;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;
	Vec3 rollPitchYaw;
	Ptr<IndexBuffer<uint16>> indexBuffer;
	Ptr<vs_shader::VertexBuffer> vertexBuffer;
	DXPtr<ID3D11RasterizerState> rasterizerState;
	DXPtr<ID3D11DepthStencilState>	depthStencilState;
	DXPtr<ID3D11BlendState>	blendState;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;
};

