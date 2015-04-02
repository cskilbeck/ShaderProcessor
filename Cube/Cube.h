//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	AssetManager mAssetManager;

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

	Ptr<Shaders::Instanced> instancedShader;
	Ptr<Shaders::Instanced::VertBuffer0> instancedVB0;
	Ptr<Shaders::Instanced::VertBuffer1> instancedVB1;

	Vec4f cubePos;
	Vec4f cubeScale;
	Vec4f cubeRot;

	Vec4f lightPos;

	Scene scene;

	float deltaTime;
	float oldDeltaTime;

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

	Ptr<Shaders::Splat> splatShader;
	Ptr<Shaders::Splat::VertBuffer> splatVB;

	Ptr<RenderTarget> fpsGraph;
	Ptr<Sampler> fpsSampler;

	Ptr<SpriteSheet> spriteSheet;

	Ptr<RenderTarget> renderTarget;
	Ptr<Shaders::Blit> blitShader;
	Ptr<Shaders::Blit::VertBuffer> blitVB;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnFrame() override;
	void OnDestroy() override;

	void CreateGrid();
	void CreateOctahedron();
};

