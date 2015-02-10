//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow
{
	//////////////////////////////////////////////////////////////////////

	FPSCamera camera;

	Ptr<Shaders::Phong::VS> vsPhong;
	Ptr<Shaders::Phong::PS> psPhong;
	Ptr<Shaders::Phong::VertBuffer> cubeVerts;
	Ptr<Texture> texture;
	Ptr<Sampler> sampler;
	Ptr<IndexBuffer<uint16>> cubeIndices;

	Ptr<Shaders::Colored::VS> vsSolidColor;
	Ptr<Shaders::Colored::PS> psSolidColor;
	Ptr<Shaders::Colored::VertBuffer> gridVB;
	Ptr<Shaders::Colored::VertBuffer> octahedronVB;
	Ptr<IndexBuffer<uint16>> octahedronIB;

	Material blendEnabled;
	Material blendDisabled;

	Vec4f cubePos;
	Vec4f cubeScale;
	Vec4f cubeRot;

	DrawList drawList;
	Ptr<Shaders::UI::VS> vsUI;
	Ptr<Shaders::UI::PS> psUI;
	Ptr<Shaders::UI::VertBuffer> UIVerts;
	Ptr<Texture> uiTexture;
	Ptr<Sampler> uiSampler;
	Material uiMaterial;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnDraw() override;
	void OnDestroy() override;

	void CreateGrid();
	void CreateOctahedron();
};

