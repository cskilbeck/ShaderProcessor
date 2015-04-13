//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

#include "Shaders/Color.shader.h"

class PhysicsDebug: public btIDebugDraw
{
public:

	PhysicsDebug();
	~PhysicsDebug();

	int Create(DX::DXWindow *window);
	void Release();

	void BeginScene(Camera *camera);
	void EndScene();

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int getDebugMode() const;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

private:

	DX::DXWindow *								mWindow;
	DX::DrawList								mDrawList;
	DXShaders::Color							mShader;
	VertexBuffer<DXShaders::Color::InputVertex>	mVertexBuffer;
	Camera *									mCamera;
	int											mDebugModes;
};
