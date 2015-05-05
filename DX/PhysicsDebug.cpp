//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

PhysicsDebug::PhysicsDebug()
	: mWindow(null)
	, mCamera(null)
{
	mDebugModes = 0;
}

//////////////////////////////////////////////////////////////////////

PhysicsDebug::~PhysicsDebug()
{
}

//////////////////////////////////////////////////////////////////////

int PhysicsDebug::Create(DX::DXWindow *window)
{
	mWindow = window;
	DXR(mShader.Create());
	DXR(mVertexBuffer.Create(8192));
	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::Release()
{
	mShader.Release();
	mVertexBuffer.Destroy();
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::BeginScene(Camera *camera)
{
	mCamera = camera;

	mDrawList.Reset(mWindow->Context(), &mShader, &mVertexBuffer);
	mDrawList.SetConstantData(DX::ShaderType::Vertex, Transpose(camera->GetTransformMatrix()), DXShaders::Color::VS::VertConstants_index);
	mDrawList.BeginLineList();
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::EndScene()
{
	mDrawList.End();
	mDrawList.Execute();
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	DXShaders::Color::InputVertex vf = { from, (float *)&color };
	DXShaders::Color::InputVertex vt = { to, (float *)&color };
	vf.Color.SetAlpha(255);
	vt.Color.SetAlpha(255);
	mDrawList.AddVertex(vf);
	mDrawList.AddVertex(vt);
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::reportErrorWarning(const char* warningString)
{
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::draw3dText(const btVector3& location, const char* textString)
{
	using namespace DirectX;
	Vec4f sp = TransformPoint(location.mVec128, mCamera->GetTransformMatrix());
	Vec2f screenPos((GetX(sp) + 1) / 2 * mWindow->FClientWidth(), (-GetY(sp) + 1) / 2 * mWindow->FClientHeight());
	debug_text((int)screenPos.x, (int)screenPos.y, textString);
}

//////////////////////////////////////////////////////////////////////

void PhysicsDebug::setDebugMode(int debugMode)
{
	mDebugModes = debugMode;
}

//////////////////////////////////////////////////////////////////////

int PhysicsDebug::getDebugMode() const
{
	return mDebugModes;
}
