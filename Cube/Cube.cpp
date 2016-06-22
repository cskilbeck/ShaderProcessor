//////////////////////////////////////////////////////////////////////
// RTCB001: don't return a bool to indicate success or failure (return a signed integer, where a negative value indicates failure)
// RTCB002: don't do anything that can fail in a constructor (unless you dig exceptions, in which case throw up all you like)
// RTCB003: when declaring function parameters, prefer enum to bool
// RTCB004: status flags should be off (0) in the default state (eg prefer Disabled = 0 to Enabled = 1, Hidden = 0 to Visible = 1)
// RTCB005: 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

static Shaders::Phong::InputVertex verts[24] =
{
	{ { -1, +1, +1 }, { 0, 0 }, 0xffffffff, { 0, 0, +1 } },// 00
	{ { +1, +1, +1 }, { 1, 0 }, 0xffffffff, { 0, 0, +1 } },// 01
	{ { +1, -1, +1 }, { 1, 1 }, 0xffffffff, { 0, 0, +1 } },// 02
	{ { -1, -1, +1 }, { 0, 1 }, 0xffffffff, { 0, 0, +1 } },// 03
	{ { -1, -1, +1 }, { 0, 0 }, 0xffffffff, { 0, -1, 0 } },// 04
	{ { +1, -1, +1 }, { 1, 0 }, 0xffffffff, { 0, -1, 0 } },// 05
	{ { +1, -1, -1 }, { 1, 1 }, 0xffffffff, { 0, -1, 0 } },// 06
	{ { -1, -1, -1 }, { 0, 1 }, 0xffffffff, { 0, -1, 0 } },// 07
	{ { -1, -1, -1 }, { 0, 0 }, 0xffffffff, { 0, 0, -1 } },// 08
	{ { +1, -1, -1 }, { 1, 0 }, 0xffffffff, { 0, 0, -1 } },// 09
	{ { +1, +1, -1 }, { 1, 1 }, 0xffffffff, { 0, 0, -1 } },// 10
	{ { -1, +1, -1 }, { 0, 1 }, 0xffffffff, { 0, 0, -1 } },// 11
	{ { -1, +1, -1 }, { 0, 0 }, 0xffffffff, { 0, +1, 0 } },// 12
	{ { +1, +1, -1 }, { 1, 0 }, 0xffffffff, { 0, +1, 0 } },// 13
	{ { +1, +1, +1 }, { 1, 1 }, 0xffffffff, { 0, +1, 0 } },// 14
	{ { -1, +1, +1 }, { 0, 1 }, 0xffffffff, { 0, +1, 0 } },// 15
	{ { +1, +1, +1 }, { 0, 0 }, 0xffffffff, { +1, 0, 0 } },// 16
	{ { +1, +1, -1 }, { 1, 0 }, 0xffffffff, { +1, 0, 0 } },// 17
	{ { +1, -1, -1 }, { 1, 1 }, 0xffffffff, { +1, 0, 0 } },// 18
	{ { +1, -1, +1 }, { 0, 1 }, 0xffffffff, { +1, 0, 0 } },// 19
	{ { -1, +1, -1 }, { 0, 0 }, 0xffffffff, { -1, 0, 0 } },// 20
	{ { -1, +1, +1 }, { 1, 0 }, 0xffffffff, { -1, 0, 0 } },// 21
	{ { -1, -1, +1 }, { 1, 1 }, 0xffffffff, { -1, 0, 0 } },// 22
	{ { -1, -1, -1 }, { 0, 1 }, 0xffffffff, { -1, 0, 0 } } // 23
};

//////////////////////////////////////////////////////////////////////

static Shaders::Instanced::InputVertex iCube[24] =
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
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

//////////////////////////////////////////////////////////////////////

MyDXWindow::MyDXWindow()
	: DXWindow(1280, 720, TEXT("Cube"), DepthBufferEnabled, Windowed)
{
	mouseClicked = false;
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

void MyDXWindow::DrawCube(Matrix const &m, VertexBuffer<Shaders::Phong::InputVertex> &cubeVerts, Texture &texture)
{
	auto &vc = *cubeShader.vs.VertConstants;
	vc.TransformMatrix = Transpose(camera->GetTransformMatrix(m));
	vc.ModelMatrix = Transpose(m);
	vc.Update(Context());
	auto &ps = *cubeShader.ps.Camera;
	ps.cameraPos = camera->position;
	ps.Update(Context());
	cubeShader.ps.picTexture = &texture;
	cubeShader.Activate(Context());
	cubeShader.vs.SetVertexBuffers(Context(), 1, &cubeVerts);
	cubeIndices.Activate(Context());
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context()->DrawIndexed(cubeIndices.Count(), 0, 0);
}

//////////////////////////////////////////////////////////////////////

const int gridSize = 500;

int MyDXWindow::CreateGrid()
{
	const int size = gridSize;
	const int gap = gridSize / 20;
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
		Color cx = 0x20FFFFFF;
		Color cy = 0x20FFFFFF;
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

static float sinc(float f)
{
	f *= PI;	// hmmm
	return sinf(f) / f;
}

// fill in the gaps, just linear for now

static vector<Vec4f> linear_interpolate(vector<Vec4f> &v, vector<uint64> &times)
{
	assert(times.size() == v.size());
	assert(times.size() > 1);
	assert(times.back() < 65536);
	vector<Vec4f> t(times.back() + 1);
	int index = 0;
	for(int i=0; i<v.size() - 1; ++i)
	{
		Vec4f s = v[i];
		Vec4f d = v[i + 1] - s;
		int64 dt = times[i + 1] - times[i];
		for(int64 j=0; j<dt; ++j)
		{
			float r = j / (float)dt;
			t[index++] = s + d * r;
		}
	}
	t[index++] = v.back();	// tack on the last one
	return t;
}

bool MyDXWindow::LoadCSV()
{
	int ignored = 0;
	int lines = 0;

	uint64 timebase;

	DiskFile f;
	DXB(f.Open(TEXT("Data\\DataSet1 -- log_20151209_161631.csv"), DiskFile::ForReading));
	while (true)
	{
		tstring line;
		if(f.ReadLine(line) != 0)				// get a line from the file
		{
			break;
		}
		vector<tstring> tok;
		tokenize(line, tok, TEXT(","));			// split on the commas
		if (lines > 0)							// ignore header line
		{
			if (tok.size() != 8)				// ignore any with != 6 values
			{
				ignored++;
			}
			else
			{
				float f[8];
				int i = 0;
				for (auto &t : tok)
				{
					t = Trim(t);
					f[i++] = (float)atof(t.c_str());		// trim the values and convert to floats
				}
				mAccel.push_back(Vec4(f[2], f[4], f[3]));		// get the gyro
				mGyro.push_back(Vec4(f[5], f[7], f[6]));		// get the accel

				uint h, m, s, ms;

				_stscanf_s(tok[1].c_str(), TEXT("%d:%d:%d.%d"), &h, &m, &s, &ms);	// get the time

				uint64 total = (uint64)ms + (uint64)s * 1000 + (uint64)m * 60 * 1000 + (uint64)h * 60 * 60 * 1000;

				if (lines == 1)
				{
					timebase = total;
				}
				total -= timebase;							// offset to zero based milliseconds

				mTimes.push_back(total);

				TRACE(TEXT("Gyro: %f,%f,%f, Accel: %f,%f,%f, Time: %I64d\n"),
					GetX(mGyro.back()), GetY(mGyro.back()), GetZ(mGyro.back()),
					GetX(mAccel.back()), GetY(mAccel.back()), GetZ(mAccel.back()),
					mTimes.back());
			}
		}
		++lines;
	}
	if (ignored > 0)
	{
		MessageBox(NULL, Format(TEXT("Warning, %d lines ignored..."), ignored).c_str(), TEXT("Loading file"), MB_ICONEXCLAMATION);
	}

	// now create interpolated data at Nms intervals
	// and for fancy pants mode, use sin(x)/x
	mGyroInterpolated = linear_interpolate(mGyro, mTimes);
	mAccelInterpolated = linear_interpolate(mAccel, mTimes);

	return true;
}

//////////////////////////////////////////////////////////////////////

bool MyDXWindow::OnCreate()
{
	TRACE("=== OnCreate() ===\n");

	LoadCSV();

	if(!DXWindow::OnCreate())
	{
		return false;
	}

	mKeyPressed = [this] (KeyboardEvent const &e)
	{
		//Trace("[%04x] : %c\n", e.key, e.key);
		if(e.key == VK_F4)
		{
			mD3D.ToggleFullScreen();
		}
	};

	KeyPressed += mKeyPressed;

	camera = new FPSCamera(this);

	Load();

	AssetManager::AddFolder("Data");
	AssetManager::AddArchive("data.zip");

	DXB(cubeShader.Create());
	DXB(cubeTexture.Load(TEXT("temp.jpg")));
	Sampler::Options o;
	o.Filter = anisotropic;
	DXB(cubeSampler.Create(o));
	cubeShader.ps.picTexture = &cubeTexture;
	cubeShader.ps.tex1Sampler = &cubeSampler;
	DXB(cubeIndices.Create(_countof(indices), indices, StaticUsage));
	DXB(cubeVerts.Create(_countof(verts), verts, StaticUsage));

	UI::Open(this);

	DXB(FontManager::Open(this));

	DXB(debug_open(this));

	DXB(FontManager::Load(TEXT("debug"), &font));
	DXB(FontManager::Load(TEXT("Cooper_Black_48"), &bigFont));

	DXB(fontVB.Create(8192));
	DXB(bigFontVB.Create(8192));

	DXB(simpleShader.Create());
	DXB(simpleVB.Create(128));

	DXB(CreateGrid());

	DXB(buttonTexture.Load("button.png"));

	Resized += mWindowSized = [this] (WindowSizedEvent const &s)
	{
		root.SetSize(FClientSize());
	};

	//filledRectangle.SetColor(0x800000ff);
	//filledRectangle.SetSize({ 200, 200 });
	//filledRectangle.SetPivot(Vec2f::half);
	//filledRectangle.SetPosition(FClientSize() / 2);
	//root.AddChild(filledRectangle);

	//clipRect.SetPivot(Vec2f::half);
	//clipRect.SetSize({ 200, 200 });
	//clipRect.SetPosition(FClientSize() / 2);
	//root.AddChild(clipRect);

	//button.SetFont(font);
	//button.SetText("Click Me !");
	//button.SetImage(&buttonTexture);
	//button.SetSampler(&cubeSampler);
	//button.SetPosition({ 100, 100 });
	//clipRect.AddChild(button);

	//outlineRectangle.SetColor(Color::BrightGreen);
	//outlineRectangle.SetSize(filledRectangle.GetSize());
	//outlineRectangle.SetPivot(Vec2f::half);
	//outlineRectangle.SetPosition(FClientSize() / 2);
	//root.AddChild(outlineRectangle);

	config_element.SetPosition({ 10, 500 });

	struct
	{
		char const *name;
		float value;
		Color color;
	}
	setup[3] = {
		{ "X Scale", 0, Color::BrightRed },
		{ "Y Scale", 0.005f, Color::BrightGreen },
		{ "Z Scale", 0, Color::Cyan },
	};
	for(int i = 0; i < 3; ++i)
	{
		scale_slider[i].SetFont(font);
		scale_slider[i].SetText(setup[i].name);
		scale_slider[i].SetRange(0, 0.01f);
		scale_slider[i].SetPosition({ 0, (float)i * 30 });
		scale_slider[i].SetWidth(200);
		scale_slider[i].SetValue(setup[i].value);
		scale_slider[i].SetColor(setup[i].color);
		config_element.AddChild(scale_slider[i]);
	}

	speed_slider.SetFont(font);
	speed_slider.SetText("Speed");
	speed_slider.SetRange(0, 1.0f);
	speed_slider.SetPosition({ 0, 90 });
	speed_slider.SetWidth(200);
	speed_slider.SetValue(1);
	config_element.AddChild(speed_slider);

	position_slider.SetFont(font);
	position_slider.SetText("T");
	position_slider.SetRange(0, (float)mAccelInterpolated.size() - 1);
	position_slider.SetPosition({ 0, 120 });
	position_slider.SetWidth(1200);
	position_slider.SetValue(0);
	config_element.AddChild(position_slider);

	trim_slider.SetFont(font);
	trim_slider.SetText("Trim");
	trim_slider.SetRange(0, 1000);
	trim_slider.SetPosition({ 0, 150 });
	trim_slider.SetWidth(1200);
	trim_slider.SetValue(0);
	config_element.AddChild(trim_slider);

	mPaused = false;

	pause_button.SetFont(font);
	pause_button.SetText("Pause");
	pause_button.SetPosition({ 350, 0 });
	pause_button.SetImage(&buttonTexture);
	pause_button.SetSampler(&cubeSampler);
	pause_button.Clicked += [this] (UI::ClickedEvent e)
	{
		mPaused = !mPaused;
		pause_button.SetText(mPaused ? "Play" : "Pause");
	};
	config_element.AddChild(pause_button);

	for(int i=0; i<3; ++i)
	{
		clamp_button[i].SetFont(font);
		clamp_button[i].SetImage(&buttonTexture);
		clamp_button[i].SetText(Format("Clamp %c", 'X' + i).c_str());
		clamp_button[i].SetPosition({ 350, 40 + i * 30.0f });
		clamp_button[i].SetSampler(&cubeSampler);
		clamp_button[i].mTag = i;
		clamp[i] = false;
		clamp_button[i].Clicked += [this](UI::ClickedEvent e)
		{
			UI::LabelButton &l = (UI::LabelButton &)*e.mElement;
			clamp[l.mTag] = !clamp[l.mTag];
			l.SetText(Format("%s %c", clamp[l.mTag] ? "Unclamp" : "Clamp", 'X' + l.mTag).c_str());
		};
		config_element.AddChild(clamp_button[i]);
	}

	root.AddChild(config_element);

	root.Pressed += [this] (UI::MouseEvent const &m)
	{
		mouseClicked = true;
	};

	// this should never happen...
	root.Released += [this] (UI::MouseEvent const &m)
	{
		mouseClicked = false;
	};

	auto remover = [] (UI::ClickedEvent const &e)
	{
		e.mElement->Close();
	};

	//listBox.SetPosition({ 200, 200 });
	//listBox.SetSize({ 200, 300 });

	//for(int i = 0; i < 30; ++i)
	//{
	//	UI::Element *e = listBox.AddString(Format("Hello %*d", Random::UInt32() & 31, Random::UInt32()).c_str(), font);
	//	e->Clicked += remover;
	//}

	//KeyPressed += [this, &remover] (KeyboardEvent const &k)
	//{
	//	UI::Element *e = listBox.AddString(Format("Hello %*d", Random::UInt32() & 31, Random::UInt32()).c_str(), font);
	//	e->Clicked += remover;
	//};
	//root.AddChild(listBox);

	//button.MouseEntered += [] (UI::MouseEvent e)
	//{
	//	UI::LabelButton *b = (UI::LabelButton *)e.mElement;
	//	b->SetScale(Vec2f(1.1f, 1.1f));
	//	b->SetText("ENTER!");
	//};

	//button.MouseLeft += [] (UI::MouseEvent e)
	//{
	//	UI::LabelButton *b = (UI::LabelButton *)e.mElement;
	//	b->SetScale(Vec2f(1, 1));
	//	b->SetText("LEAVE!");
	//};

	//button.Pressed += [] (UI::PressedEvent e)
	//{
	//	UI::LabelButton *b = (UI::LabelButton *)e.mElement;
	//	b->SetText("PRESSED!");
	//};

	lightPos = Vec4(0, -15, 20, 0);

	DXB(uiShader.Create());
	DXB(UIVerts.Create(12));
	DXB(uiTexture.Load(TEXT("temp.png")));
	DXB(uiSampler.Create());
	uiShader.ps.page = &uiTexture;
	uiShader.ps.smplr = &uiSampler;
	uiShader.vs.vConstants->TransformMatrix = Transpose(OrthoProjection2D(ClientWidth(), ClientHeight()));
	DXB(uiShader.vs.vConstants->Update());
	uiShader.ps.pConstants->Color = Float4(1, 1, 1, 1);
	DXB(uiShader.ps.pConstants->Update());


	TRACE("=== End of OnCreate() ===\n");

	mCurrentTime.Reset();
	mPlaybackTime = 0.0;

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnFrame()
{
	debug_begin();

	oldDeltaTime = deltaTime;

	deltaTime = mTimer.Delta();

	mPlaybackTime += deltaTime * speed_slider.GetValue();
	float time = (float)mTimer.WallTime();

	mTimer.Update();

	camera->Process((float)deltaTime);

	debug_set_camera(*camera);

	float const gyro_scale = PI / 180 / (65536.0f / 50.0f);

	//debug_cylinder(Vec4(10, 10, 10), Vec4(50, 10, 10 + sinf(time) * 50), 2, Color::BrightRed);
	//debug_line2d({ 0, 0 }, FClientSize() * 0.6666f, Color::Random());
	//debug_rect2d({ 100, 100 }, { 200, 200 }, Color::Random());
	//debug_filled_rect2d({ 100, 100 }, { 600, 400 }, 0x80ff0000, 0xff00ff00);

	Clear(Color(16, 32, 64));
	ClearDepth(DepthOnly, 1.0f, 0);

	// Draw grid

	simpleShader.Activate(Context());
	simpleShader.vs.VertConstants->Get()->TransformMatrix = Transpose(camera->GetTransformMatrix());
	simpleShader.vs.SetVertexBuffers(Context(), 1, &gridVB);
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(gridVB.Count(), 0);

	debug_text("DeltaTime % 8.2fms (% 3dfps)\n", deltaTime * 1000, (int)(1 / deltaTime));

	float ascale = 0.01f;
	float gscale = 0.0004f;
	Vec4f scale = Vec4(scale_slider[0].GetValue(), scale_slider[1].GetValue(), scale_slider[2].GetValue());

	float t = (float)mPlaybackTime * 1000.0f;

	if(!mPaused)
	{
		position_slider.SetValue(fmodf((float)(mPlaybackTime * 1000.0), (float)mAccelInterpolated.size()));
	}
	else
	{
		t = position_slider.GetValue() + trim_slider.GetValue();
	}

	UI::Update(&root, (float)deltaTime);
	UI::Draw(&root, Context(), drawList, OrthoProjection2D(ClientWidth(), ClientHeight()));
	const int rate = 32;

	auto gyroMatrix = [&] (Vec4f g)
	{
		Vec4f s = g * gyro_scale;
		Matrix x = clamp[0] ? IdentityMatrix : DirectX::XMMatrixRotationX(GetX(s));
		Matrix y = clamp[1] ? IdentityMatrix : DirectX::XMMatrixRotationY(GetY(s));
		Matrix z = clamp[2] ? IdentityMatrix : DirectX::XMMatrixRotationZ(GetZ(s));
		return z * y * x;
	};

	auto gyro2ypr = [&gyroMatrix] (Vec4f g)
	{
		return TransformPoint(Vec4(0, 10, 0), gyroMatrix(g));
	};

	Vec4f gorg = Vec4(30, 0, 0);

	Vec4f s = gyro2ypr(mGyroInterpolated[0]) + gorg;
	for(int i = 1; i < mGyroInterpolated.size(); ++i)
	{
		Vec4f e = gyro2ypr(mGyroInterpolated[i]) + scale * (float)i + gorg;
		debug_line(s, e, Color::BrightRed);
		s = e;
	}
	drawList.Execute();

	//debug_quad(Vec4(-60, 20, 0), Vec4(-50, 20, 0), Vec4(-50, 30, 0), Vec4(-60, 30, 0), Color::Cyan);

	// Draw some sprites immediate mode

	debug_end();

	debug_begin();
	debug_set_camera(*camera);
	s = mAccelInterpolated[0] * ascale;
	for(int i = 1; i < mAccelInterpolated.size(); ++i)
	{
		Vec4f e = mAccelInterpolated[i] * ascale + scale * (float)i;
		debug_line(s, e, 0xFF40FF80);
		s = e;
	}

	// draw little lines/cubes at the current time

	int ms = (int)fmodf(t, (float)mAccelInterpolated.size());
	Vec4f sz = Vec4(0.5f, 0.5f, 0.5f);
	Vec4f pos = mAccelInterpolated[ms] * ascale + scale * (float)ms;
	debug_cube(pos - sz, pos + sz, Color::White);
	debug_line(pos, scale * (float)ms, Color::Cyan);

	pos = gyro2ypr(mGyroInterpolated[ms]) + scale * (float)ms + gorg;
	debug_cube(pos - sz, pos + sz, Color::Yellow);
	debug_line(pos, scale * (float)ms + gorg, Color::Cyan);

	drawList.Execute();

	{
		auto &l = *cubeShader.ps.Light;
		l.lightPos = lightPos;
		l.ambientColor = Float3(0.3f, 0.3f, 0.3f);
		l.diffuseColor = Float3(0.7f, 0.7f, 0.7f);
		l.specColor = Float3(5, 5, 5);
		l.Update(Context());
	}

	DrawCube(gyroMatrix(mGyroInterpolated[ms]) * ScaleMatrix(Vec4(5, 5, 5)) * TranslationMatrix(Vec4(50, 0, 0)), cubeVerts, cubeTexture);

	debug_end();
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	TRACE("=== BEGINNING OF OnDestroy() ===\n");

	Save();

	UI::Close();

	debug_close();

	cubeShader.Release();
	cubeVerts.Destroy();
	cubeIndices.Destroy();
	cubeTexture.Release();
	cubeSampler.Release();

	buttonTexture.Release();
	fontVB.Destroy();
	bigFontVB.Destroy();

	simpleShader.Release();
	gridVB.Destroy();
	simpleVB.Destroy();
	uiShader.Release();
	UIVerts.Destroy();
	uiTexture.Release();
	uiSampler.Release();

	font.Release();
	bigFont.Release();

	Scene::CleanUp();
	Texture::FlushAll();
	FontManager::Close();

	AssetManager::Close();

	TRACE("=== END OF OnDestroy() ===\n");
}

//////////////////////////////////////////////////////////////////////

void FPSCamera::Process(float deltaTime)
{
	float moveSpeed = 200.0f * deltaTime;
	float strafeSpeed = 200.0f * deltaTime;

	if(Keyboard::Held(VK_SHIFT))
	{
		moveSpeed *= 0.01f;
		strafeSpeed *= 0.01f;
	}

	v4 move = v4::zero();
	if(Keyboard::Held('A')) {
		move.x += -strafeSpeed;
	}
	if(Keyboard::Held('D')) { move.x += strafeSpeed; }
	if(Keyboard::Held('W')) { move.y += moveSpeed; }
	if(Keyboard::Held('S')) { move.y += -moveSpeed; }
	if(Keyboard::Held('R')) { move.z += strafeSpeed; }
	if(Keyboard::Held('F')) { move.z += -strafeSpeed; }

	if(Keyboard::Pressed('X'))
	{
		position = Vec4(0, 200, 0);
		this->LookAt(Vec4(0, 0, 0));
	}

	if(Keyboard::Pressed('Y'))
	{
		position = Vec4(200, 0, 0);
		this->LookAt(Vec4(0, 0, 0));
	}

	if(Keyboard::Pressed('Z'))
	{
		position = Vec4(0, 0, 200);
		this->LookAt(Vec4(0, 0, 0));
	}

	Move(move);

	if(window->mouseClicked)
	{
		if(Mouse::Held == 0)
		{
			window->mouseClicked = false;
			Mouse::SetMode(Mouse::Mode::Free, *window);
		}
		else
		{
			Mouse::SetMode(Mouse::Mode::Captured, *window);
			Rotate(Mouse::Delta.x * 0.001f, Mouse::Delta.y * 0.001f);
		}
	}

	CalculatePerspectiveProjectionMatrix(0.5f, window->FClientWidth() / window->FClientHeight());
	CalculateViewMatrix(position, yaw, pitch, roll);
	CalculateViewProjectionMatrix();
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::Load()
{
	DiskFile f;
	if(f.Open("camera.bin", DiskFile::Mode::ForReading) == S_OK)
	{
		camera->Read(f);
	}
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::Save()
{
	DiskFile f;
	if(f.Create("camera.bin", DiskFile::Overwrite) == S_OK)
	{
		camera->Write(f);
	}
}
