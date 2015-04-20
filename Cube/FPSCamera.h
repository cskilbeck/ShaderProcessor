//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow;

struct DynamicCamera : Camera
{
	Vec4f position;

	DynamicCamera()
		: position(Vec4(0, -150, 250))
	{
	}

	virtual void Process(float deltaTime) = 0;
	virtual void Read(FileBase &f) = 0;
	virtual void Write(FileBase &f) = 0;
};

struct FPSCamera: DynamicCamera
{
	float yaw;
	float pitch;
	float roll;
	MyDXWindow *window;

	FPSCamera(MyDXWindow *w)
		: DynamicCamera()
		, yaw(0)
		, pitch(-PI / 4)
		, roll(0)
		, window(w)
	{
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
		position += GetFlatForwardVector() * -GetY(direction);
		position += Vec4(0, 0, GetZ(direction));
	}

	void Rotate(float pan, float tilt)
	{
		pitch = Constrain(pitch + tilt, -PI + FLT_EPSILON, -FLT_EPSILON);
		yaw += pan;
	}

	void Process(float deltaTime) override;

	void Write(FileBase &f) override
	{
		f.Put(yaw);
		f.Put(pitch);
		f.Put(roll);
		f.Put(position);
	}

	void Read(FileBase &f) override
	{
		f.Get(yaw);
		f.Get(pitch);
		f.Get(roll);
		f.Get(position);
	}
};

