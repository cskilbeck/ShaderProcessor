//////////////////////////////////////////////////////////////////////
// RTCB001: don't return a bool to indicate success or failure (return a signed integer, where a negative value indicates failure)
// RTCB002: don't do anything that can fail in a constructor (unless you dig exceptions, in which case throw up all you like)
// RTCB003: when declaring function parameters, prefer enum to bool
// RTCB004: status flags should be off (0) in the default state (eg prefer Disabled = 0 to Enabled = 1, Hidden = 0 to Visible = 1)
// RTCB005: 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

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
		Color cx = 0x40FFFFFF;
		Color cy = 0x40FFFFFF;
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
				mGyro.push_back(Vec4(f[2], f[3], f[4]));		// get the gyro
				mAccel.push_back(Vec4(f[5], f[6], f[7]));	// get the accel

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
	vector<Vec4f> lg;
	vector<Vec4f> la;

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

	filledRectangle.SetColor(0x800000ff);
	filledRectangle.SetSize({ 200, 200 });
	filledRectangle.SetPivot(Vec2f::half);
	filledRectangle.SetPosition(FClientSize() / 2);
	root.AddChild(filledRectangle);

	clipRect.SetPivot(Vec2f::half);
	clipRect.SetSize({ 200, 200 });
	clipRect.SetPosition(FClientSize() / 2);
	root.AddChild(clipRect);

	button.SetFont(font);
	button.SetText("Click Me !");
	button.SetImage(&buttonTexture);
	button.SetSampler(&cubeSampler);
	button.SetPosition({ 100, 100 });
	clipRect.AddChild(button);

	outlineRectangle.SetColor(Color::BrightGreen);
	outlineRectangle.SetSize(filledRectangle.GetSize());
	outlineRectangle.SetPivot(Vec2f::half);
	outlineRectangle.SetPosition(FClientSize() / 2);

	root.AddChild(outlineRectangle);

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

	listBox.SetPosition({ 200, 200 });
	listBox.SetSize({ 200, 300 });

	for(int i = 0; i < 30; ++i)
	{
		UI::Element *e = listBox.AddString(Format("Hello %*d", Random::UInt32() & 31, Random::UInt32()).c_str(), font);
		e->Clicked += remover;
	}

	KeyPressed += [this, &remover] (KeyboardEvent const &k)
	{
		UI::Element *e = listBox.AddString(Format("Hello %*d", Random::UInt32() & 31, Random::UInt32()).c_str(), font);
		e->Clicked += remover;
	};
	root.AddChild(listBox);

	button.MouseEntered += [] (UI::MouseEvent e)
	{
		UI::LabelButton *b = (UI::LabelButton *)e.mElement;
		b->SetScale(Vec2f(1.1f, 1.1f));
		b->SetText("ENTER!");
	};

	button.MouseLeft += [] (UI::MouseEvent e)
	{
		UI::LabelButton *b = (UI::LabelButton *)e.mElement;
		b->SetScale(Vec2f(1, 1));
		b->SetText("LEAVE!");
	};

	button.Pressed += [] (UI::PressedEvent e)
	{
		UI::LabelButton *b = (UI::LabelButton *)e.mElement;
		b->SetText("PRESSED!");
	};

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

	return true;
}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnFrame()
{
	debug_begin();

	oldDeltaTime = deltaTime;
	deltaTime = (float)mTimer.Delta();
	float time = (float)mTimer.WallTime();

	camera->Process(deltaTime);

	debug_set_camera(*camera);

	//debug_cylinder(Vec4(10, 10, 10), Vec4(50, 10, 10 + sinf(time) * 50), 2, Color::BrightRed);
	//debug_line2d({ 0, 0 }, FClientSize() * 0.6666f, Color::Random());
	//debug_rect2d({ 100, 100 }, { 200, 200 }, Color::Random());
	//debug_filled_rect2d({ 100, 100 }, { 600, 400 }, 0x80ff0000, 0xff00ff00);

	Clear(Color(32, 64, 128));
	ClearDepth(DepthOnly, 1.0f, 0);

	// Draw grid

	simpleShader.Activate(Context());
	simpleShader.vs.VertConstants->Get()->TransformMatrix = Transpose(camera->GetTransformMatrix());
	simpleShader.vs.SetVertexBuffers(Context(), 1, &gridVB);
	Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Context()->Draw(gridVB.Count(), 0);

	debug_text("DeltaTime % 8.2fms (% 3dfps)\n", deltaTime * 1000, (int)(1 / deltaTime));

	UI::Update(&root, deltaTime);
	UI::Draw(&root, Context(), drawList, OrthoProjection2D(ClientWidth(), ClientHeight()));

	drawList.Execute();

	//debug_quad(Vec4(-60, 20, 0), Vec4(-50, 20, 0), Vec4(-50, 30, 0), Vec4(-60, 30, 0), Color::Cyan);

	// Draw some sprites immediate mode

	debug_end();

}

//////////////////////////////////////////////////////////////////////

void MyDXWindow::OnDestroy()
{
	TRACE("=== BEGINNING OF OnDestroy() ===\n");

	Save();

	UI::Close();

	debug_close();

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
	float moveSpeed = 50.0f * deltaTime;
	float strafeSpeed = 50.0f * deltaTime;

	if(Keyboard::Held(VK_SHIFT))
	{
		moveSpeed *= 0.1f;
		strafeSpeed *= 0.1f;
	}

	v4 move = v4::zero();
	if(Keyboard::Held('A')) { move.x += -strafeSpeed; }
	if(Keyboard::Held('D')) { move.x += strafeSpeed; }
	if(Keyboard::Held('W')) { move.y += moveSpeed; }
	if(Keyboard::Held('S')) { move.y += -moveSpeed; }
	if(Keyboard::Held('R')) { move.z += strafeSpeed; }
	if(Keyboard::Held('F')) { move.z += -strafeSpeed; }

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

