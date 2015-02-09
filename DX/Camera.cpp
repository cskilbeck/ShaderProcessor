//////////////////////////////////////////////////////////////////////

#include "DX.h"

//////////////////////////////////////////////////////////////////////

namespace DX
{

	Camera::Camera()
	{
		CalculateViewMatrix(Vec4(0, 0, 0), 0, 0, 0);
		CalculatePerspectiveProjectionMatrix();
	}

	//////////////////////////////////////////////////////////////////////

	Matrix Camera::ViewMatrix(Vec4f target, Vec4f position, Vec4f up)
	{
		using namespace DirectX;
		Vec4f direction(target - position);
		Vec4f Z = Normalize(direction);
		Vec4f X = Normalize(Cross(up, Z));
		Vec4f Y = Cross(Z, X);
		Vec4f nPosition = Negate(position);
		Vec4f dx = Splat(Dot(X, nPosition));
		Vec4f dy = Splat(Dot(Y, nPosition));
		Vec4f dz = Splat(Dot(Z, nPosition));
		Matrix m;
		m.r[0] = Select(dx, Negate(X), gMMaskXYZ);
		m.r[1] = Select(dy, Negate(Y), gMMaskXYZ);
		m.r[2] = Select(dz, Negate(Z), gMMaskXYZ);
		m.r[3] = IdentityMatrix.r[3];
		return Transpose(m);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix Camera::ViewMatrix(Vec4f position, float roll, float pitch, float yaw)
	{
		Matrix z = IdentityMatrix;	// Z UP not down...?
		z.r[2] = Negate(z.r[2]);

		Matrix m = IdentityMatrix;
		m.r[3] = SetW(Negate(position), 1.0f);

		// we use Z up, not Y, so swap roll, yaw
		return m * z * DirectX::XMMatrixRotationRollPitchYaw(pitch, roll, yaw);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix Camera::PerspectiveProjection(float fov /* = 0.5f */, float aspectRatio /* = 4.0f / 3.0f */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
	{
		using namespace DirectX;
		float s;
		float c;
		XMScalarSinCos(&s, &c, fov * 0.5f);
		float h = c / s;
		float w = h / aspectRatio;
		float f = farZ / (farZ - nearZ);
		float n = -f * nearZ;
		return Matrix(w, 0, 0, 0,
					  0, h, 0, 0,
					  0, 0, f, 1,
					  0, 0, n, 0);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix Camera::OrthoProjection3D(float width /* = 40 */, float height /* = 30 */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
	{
		float high = 2 / height;
		float wide = 2 / width;
		float zDiff = farZ - nearZ;
		float zF = 1 / zDiff;
		float zN = -nearZ / zDiff;
		return Matrix(wide, 0, 0, 0,
					  0, high, 0, 0,
					  0, 0, zF, 0,
					  0, 0, zN, 1);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix Camera::OrthoProjection2D(int width, int height)
	{
		float hw = 2.0f / width;
		float hh = -2.0f / height;
		return Matrix(hw, 0, 0, 0,
					  0, hh, 0, 0,
					  0, 0, 1, 0,
					  -1, 1, 0, 1);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewMatrix(Vec4f target, Vec4f position, Vec4f up)
	{
		mViewMatrix = ViewMatrix(target, position, up);
	}

	//////////////////////////////////////////////////////////////////////

	void Camera::CalculateViewMatrix(Vec4f position, float yaw, float pitch, float roll)
	{
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