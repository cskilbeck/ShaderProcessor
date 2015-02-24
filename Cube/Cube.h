//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	FPSCamera camera;
	FPSCamera dashCam;

	Ptr<Shaders::Phong> cubeShader;
	Ptr<Shaders::Phong::VertBuffer> cubeVerts;
	Ptr<IndexBuffer<uint16>> cubeIndices;
	Ptr<Texture> cubeTexture;
	Ptr<Sampler> cubeSampler;

	Ptr<Shaders::Simple> simpleShader;
	Ptr<Shaders::Simple::VertBuffer> gridVB;
	Ptr<Shaders::Simple::VertBuffer> octahedronVB;
	Ptr<Shaders::Simple::VertBuffer> simpleVB;
	Ptr<IndexBuffer<uint16>> octahedronIB;

	Vec4f cubePos;
	Vec4f cubeScale;
	Vec4f cubeRot;

	Scene scene;

	DrawList drawList;

	Ptr<Shaders::UI> uiShader;
	Ptr<Shaders::UI::VertBuffer> UIVerts;
	Ptr<Texture> uiTexture;
	Ptr<Sampler> uiSampler;

	Ptr<Font> font;
	Ptr<Font> bigFont;

	Ptr<Shaders::Sprite> spriteShader;
	Ptr<Shaders::Sprite::VertBuffer> spriteVerts;
	Ptr<Texture> spriteTexture;
	Ptr<Sampler> spriteSampler;

	Ptr<SpriteSheet> spriteSheet;

	Ptr<RenderTarget> renderTarget;
	Ptr<Shaders::Blit> blitShader;
	Ptr<Shaders::Blit::VertBuffer> blitVB;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;

	void CreateGrid();
	void CreateOctahedron();
};

