//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	FPSCamera camera;
	FPSCamera dashCam;

	Shaders::Phong cubeShader;
	Shaders::Phong::VertBuffer cubeVerts;
	IndexBuffer<uint16> cubeIndices;
	Texture cubeTexture;
	Sampler cubeSampler;

	Shaders::Simple simpleShader;
	Shaders::Simple::VertBuffer gridVB;
	Shaders::Simple::VertBuffer octahedronVB;
	Shaders::Simple::VertBuffer simpleVB;
	IndexBuffer<uint16> octahedronIB;

	Shaders::Instanced instancedShader;
	Shaders::Instanced::VertBuffer0 instancedVB0;
	Shaders::Instanced::VertBuffer1 instancedVB1;

	Vec4f cubePos;
	Vec4f cubeScale;
	Vec4f cubeRot;

	Vec4f lightPos;

	Scene scene;

	float deltaTime;
	float oldDeltaTime;

	DrawList drawList;

	Shaders::UI uiShader;
	Shaders::UI::VertBuffer UIVerts;
	Texture uiTexture;
	Sampler uiSampler;

	DXPtr<Font> font;
	DXPtr<Font> bigFont;

	Shaders::Sprite spriteShader;
	Shaders::Sprite::VertBuffer spriteVerts;
	Texture spriteTexture;
	Sampler spriteSampler;

	Shaders::Splat splatShader;
	Shaders::Splat::VertBuffer splatVB;

	RenderTarget fpsGraph;
	Sampler fpsSampler;

	Ptr<SpriteSheet> spriteSheet;

	RenderTarget renderTarget;
	Shaders::Blit blitShader;
	Shaders::Blit::VertBuffer blitVB;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnFrame() override;
	void OnDestroy() override;

	int CreateGrid();
	int CreateOctahedron();
};

