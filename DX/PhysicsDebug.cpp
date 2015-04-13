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
	mVertexBuffer.Release();
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
	DXShaders::Color::InputVertex v;
	v.Color = Color((float *)&color);
	v.Color.SetAlpha(255);
	v.Position = from;
	mDrawList.AddVertex(v);
	v.Position = to;
	mDrawList.AddVertex(v);
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
	XMVECTOR v = XMVectorSet(location.x(), location.y(), location.z(), 1.0f);
	XMVECTOR sp = XMVector3TransformCoord(v, mCamera->GetTransformMatrix());
//	Vec2 screenPos((XMVectorGetX(sp) + 1) / 2 * Graphics::FWidth(), (-XMVectorGetY(sp) + 1) / 2 * Graphics::FHeight());
//	mDebugFont->DrawString(mSpriteList, textString, screenPos, Font::HCentre, Font::VCentre);
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
