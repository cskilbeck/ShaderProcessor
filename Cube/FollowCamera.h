//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct FollowCamera: Camera
{
	Vec4f position;
	Vec4f target;
	Window *window;

	FollowCamera(Window *w)
		: position(Vec4(0, -150, 250))
		, target(Vec4(0, 0, 0))
		, window(w)
	{
		CalculatePerspectiveProjectionMatrix(0.5f, (float)w->ClientWidth() / w->ClientHeight());
		Update();
	}

	void Update()
	{
		if(Length(target - position) < 30)
		{
			position = target - Normalize(position - target * 30);
		}

		CalculateViewMatrix(target, position, Vec4(0, 0, 1));
		CalculateViewProjectionMatrix();
	}

	void Save()
	{
		DiskFile f;
		if(f.Create("followcamera.bin", DiskFile::Overwrite) != S_OK)
		{
			return;
		}
		f.Write(this, sizeof(*this) - sizeof(Window *));
	}

	void Load()
	{
		DiskFile f;
		if(f.Open("followcamera.bin", DiskFile::Mode::ForReading) != S_OK)
		{
			return;
		}
		f.Read(this, sizeof(*this) - sizeof(Window *));
	}
};

