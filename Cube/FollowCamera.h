//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow;

struct FollowCamera: DynamicCamera
{
	Vec4f target;
	float distance;
	float height;
	float targetHeight;
	MyDXWindow *window;

	FollowCamera(MyDXWindow *w)
		: DynamicCamera()
		, target(Vec4(0, 0, 0))
		, window(w)
		, distance(50)
		, height(30)
		, targetHeight(30)
	{
	}

	void Process(float deltaTime) override;

	void Read(FileBase &f) override
	{
		f.Get(distance);
		f.Get(height);
		f.Get(targetHeight);
	}

	void Write(FileBase &f) override
	{
		f.Put(distance);
		f.Put(height);
		f.Put(targetHeight);
	}
};

