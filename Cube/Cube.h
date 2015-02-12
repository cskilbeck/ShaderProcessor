//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	FPSCamera camera;

	Ptr<Shaders::Phong> phongShader;
	Ptr<Shaders::Phong::VertBuffer> cubeVerts;
	Ptr<IndexBuffer<uint16>> cubeIndices;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;

	Ptr<Shaders::Colored> coloredShader;
	Ptr<Shaders::Colored::VertBuffer> gridVB;
	Ptr<Shaders::Colored::VertBuffer> octahedronVB;
	Ptr<IndexBuffer<uint16>> octahedronIB;

	Material blendEnabled;
	Material blendDisabled;

	Vec4f cubePos;
	Vec4f cubeScale;
	Vec4f cubeRot;

	DrawList drawList;
	Ptr<Shaders::UI> uiShader;
	Ptr<Shaders::UI::VertBuffer> UIVerts;
	Ptr<Texture> uiTexture;
	Ptr<Sampler> uiSampler;
	Material uiMaterial;
	Font *font;

	Ptr<Shaders::Sprite> spriteShader;
	Ptr<Shaders::Sprite::VertBuffer> spriteVerts;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;

	void CreateGrid();
	void CreateOctahedron();
};

