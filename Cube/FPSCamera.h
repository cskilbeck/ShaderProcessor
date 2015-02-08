//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct FPSCamera: Camera
{
	Vec4f position;
	float yaw;
	float pitch;
	float roll;

	FPSCamera()
		: position(Vec4(0, 0, 10))
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

