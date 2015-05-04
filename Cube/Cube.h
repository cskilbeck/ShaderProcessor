//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct MyDXWindow: DXWindow, iPhysicsRenderer
{
	//////////////////////////////////////////////////////////////////////

	DynamicCamera *cameras[2];
	DynamicCamera *camera;
	int currentCamera;
	Camera dashCam;

	void Load();
	void Save();

	Shaders::Phong cubeShader;
	Shaders::Phong::VertBuffer cubeVerts;
	Shaders::Phong::VertBuffer diceVertBuffer;
	IndexBuffer<uint16> cubeIndices;
	Texture cubeTexture;
	Texture diceTexture;
	Sampler cubeSampler;

	Shaders::Sphere sphereShader;
	Shaders::Sphere::VertBuffer sphereVerts;
	Texture sphereTexture;

	Shaders::Phong::VertBuffer cylinderVerts;
	IndexBuffer<uint16> cylinderIndices;

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

	Texture buttonTexture;

	UI::Element root;
	UI::LabelButton button;
	UI::Rectangle rectangle;
	UI::ClipRectangle clipRect;

	float deltaTime;
	float oldDeltaTime;

	bool debugPhysics;

	Vehicle car;

	DrawList drawList;

	Shaders::UI uiShader;
	Shaders::UI::VertBuffer UIVerts;
	Texture uiTexture;
	Sampler uiSampler;

	DXPtr<Typeface> font;
	DXPtr<Typeface> bigFont;

	Font::VB fontVB;
	Font::VB bigFontVB;

	Shaders::Sprite spriteShader;
	Shaders::Sprite::VertBuffer spriteVerts;
	Texture spriteTexture;
	Sampler spriteSampler;

	Shaders::Splat splatShader;
	Shaders::Splat::VertBuffer splatVB;

	RenderTarget fpsGraph;
	Sampler fpsSampler;

	SpriteSheet spriteSheet;

	RenderTarget renderTarget;
	Shaders::Blit blitShader;
	Shaders::Blit::VertBuffer blitVB;

	struct Box
	{
		Box()
			: mShape(null)
			, mBody(null)
		{
		}

		void Create(Vec4f pos);
		void Destroy();
		void Draw(MyDXWindow *window);

		btBoxShape *mShape;
		btRigidBody *mBody;
	};

	void SetupBoxes();
	void DrawCube(Matrix const &m, VertexBuffer<Shaders::Phong::InputVertex> &cubeVerts, Texture &texture);
	void DrawCylinder(Matrix const &m, Texture &texture);
	void DrawSphere(Matrix const &m, Texture &texture);

	enum
	{
		numBoxes = 25
	};
	static Box box[numBoxes];

	btBoxShape *mGroundShape;
	btRigidBody *mGroundRigidBody;

	struct PhysicalVertex
	{
		Float3 position;
		Float3 normal;
	};

	PhysicalVertex *mRampVerts;
	int32 *mRampIndices;
	btTriangleIndexVertexArray *mRampArray;
	btBvhTriangleMeshShape *mRampShape;
	btRigidBody *mRampBody;

	btCylinderShape *mCylinder;

	void SweepTest();

	Physics::World track;

	//////////////////////////////////////////////////////////////////////

	MyDXWindow();

	void OnKeyDown(int key, uintptr flags) override;
	bool OnCreate() override;
	void OnFrame() override;
	void OnDestroy() override;

	int CreateSphere(int steps);
	int CreateCylinder(int steps);
	int CreateGrid();
	int CreateOctahedron();
	int CreateRamp();

	int LoadTrack(btTransform &carTransform);

	// iPhysicsRenderer

	void DrawCube(Matrix const &m) override;
	void DrawCylinder(Matrix const &m) override;
	void DrawSphere(Matrix const &m) override;
	void DrawCapsule(Matrix const &m) override;
	void DrawTorus(Matrix const &m) override;
	void DrawCone(Matrix const &m) override;
	void DrawConvexMesh(Matrix const &m, btBvhTriangleMeshShape const *mesh) override;
	void DrawTetrahedron(Matrix const &m) override;
};
