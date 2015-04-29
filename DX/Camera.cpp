//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	Camera::Camera()
	{
		CalculateViewMatrix(Vec4(0, 0, 0), 0, 0, 0);
		CalculatePerspectiveProjectionMatrix();
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewMatrix(Vec4f target, Vec4f position, Vec4f up)
	{
		mPosition = position;
		mViewMatrix = ViewMatrix(target, position, up);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewMatrix(Vec4f position, float yaw, float pitch, float roll)
	{
		mPosition = position;
		mViewMatrix = ViewMatrix(position, yaw, pitch, roll);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculatePerspectiveProjectionMatrix(float fov /* = 0.5f */, float aspectRatio /* = 4.0f / 3.0f */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
	{
		mProjectionMatrix = PerspectiveProjection(fov, aspectRatio, nearZ, farZ);
	}
	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateOrthoProjectionMatrix(float width /* = 40 */, float height /* = 30 */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
	{
		mProjectionMatrix = OrthoProjection3D(width, height, nearZ, farZ);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateOrthoProjectionMatrix2D(int width, int height)
	{
		mProjectionMatrix = OrthoProjection2D(width, height);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewProjectionMatrix()
	{
		mTransformMatrix = mViewMatrix * mProjectionMatrix;
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewProjectionMatrix(Matrix const &modelMatrix)
	{
		mTransformMatrix = modelMatrix * mViewMatrix * mProjectionMatrix;
	}

	//////////////////////////////////////////////////////////////////////

	Vec4f Camera::GetYawPitchRoll() const
	{
		return GetPitchYawRollFromMatrix(mViewMatrix);
	}

}