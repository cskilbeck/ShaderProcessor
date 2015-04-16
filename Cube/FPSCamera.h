//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct FPSCamera: Camera
{
	Vec4f position;
	float yaw;
	float pitch;
	float roll;
	Window *window;

	FPSCamera(Window *w)
		: position(Vec4(0, -150, 250))
		, yaw(0)
		, pitch(-PI / 4)
		, roll(0)
		, window(w)
	{
		CalculatePerspectiveProjectionMatrix(0.5f, (float)w->ClientWidth() / w->ClientHeight());
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
		position += GetFlatForwardVector() * -GetY(direction);
		position += Vec4(0, 0, GetZ(direction));
	}

	void Rotate(float pan, float tilt)
	{
		pitch = Constrain(pitch + tilt, -PI + FLT_EPSILON, -FLT_EPSILON);
		yaw += pan;
	}

	void Update()
	{
		CalculateViewMatrix(position, yaw, pitch, roll);
		CalculateViewProjectionMatrix();
	}

	void Save()
	{
		DiskFile f;
		if(f.Create("camera.bin", DiskFile::Overwrite) != S_OK)
		{
			return;
		}
		f.Write(this, sizeof(*this) - sizeof(Window *));
	}

	void Load()
	{
		DiskFile f;
		if(f.Open("camera.bin", DiskFile::Mode::ForReading) != S_OK)
		{
			return;
		}
		f.Read(this, sizeof(*this) - sizeof(Window *));
	}
};

