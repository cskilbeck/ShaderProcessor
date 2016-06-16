//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	DynamicCamera *camera;
	int currentCamera;
	Camera dashCam;

	void Load();
	void Save();

	Sampler cubeSampler;

	Shaders::Simple simpleShader;
	Shaders::Simple::VertBuffer gridVB;
	VertexBuilder<Shaders::Simple::InputVertex> simpleVB;

	Vec4f lightPos;

	Texture buttonTexture;

	UI::Element root;
	UI::LabelButton button;
	UI::FilledRectangle filledRectangle;
	UI::OutlineRectangle outlineRectangle;
	UI::ClipRectangle clipRect;
	UI::ListBox listBox;
	UI::Rectangle rect;
	UI::FilledShape shape;
	UI::Slider slider;

	float deltaTime;
	float oldDeltaTime;

	bool mouseClicked;

	DrawList drawList;

	vector<Vec4f> mGyro;
	vector<Vec4f> mAccel;

	Timer		mCurrentTime;

	vector<uint64> mTimes;	//16:16:12.873

	vector<Vec4f> mGyroInterpolated;
	vector<Vec4f> mAccelInterpolated;

	Shaders::UI uiShader;
	VertexBuilder<Shaders::UI::InputVertex> UIVerts;
	Texture uiTexture;
	Sampler uiSampler;

	DXPtr<Typeface> font;
	DXPtr<Typeface> bigFont;

	Font::VB fontVB;
	Font::VB bigFontVB;

	Delegate<KeyboardEvent> mKeyPressed;
	Delegate<WindowSizedEvent> mWindowSized;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	bool LoadCSV();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnFrame() override;
	void OnDestroy() override;

	int CreateGrid();
};
