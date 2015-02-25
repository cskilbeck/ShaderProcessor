//////////////////////////////////////////////////////////////////////

#include "DXBase.h"

using namespace DirectX;

namespace DX
{
	//////////////////////////////////////////////////////////////////////

	Pool<Matrix> MatrixPool(128);

	Matrix IdentityMatrix(1, 0, 0, 0,
						  0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1);

	Matrix NegativeIdentityMatrix(-1, 0, 0, 0,
									0, -1, 0, 0,
									0, 0, -1, 0,
									0, 0, 0, 1);

	//////////////////////////////////////////////////////////////////////

	Vec4f GetPitchYawRollFromMatrix(Matrix const &m)
	{
		MatrixArray &a = (MatrixArray &)*(&m);
		float pitch = -atan2f(a[2][1], a[2][2]);
		float roll = asinf(a[2][0]);
		float yaw = -atan2f(a[1][0], a[0][0]);
		return Vec4(pitch, yaw, roll);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix ViewMatrix(Vec4f target, Vec4f position, Vec4f up)
	{
		return DirectX::XMMatrixLookAtRH(position, target, up);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix ViewMatrix(Vec4f position, float roll, float pitch, float yaw)
	{
		Matrix m = IdentityMatrix;
		m.r[3] = SetW(Negate(position), 1.0f);
		return m * DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix PerspectiveProjection(float fov /* = 0.5f */, float aspectRatio /* = 4.0f / 3.0f */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
	{
		return DirectX::XMMatrixPerspectiveFovRH(fov, aspectRatio, nearZ, farZ);
	}

	//////////////////////////////////////////////////////////////////////

	Matrix OrthoProjection3D(float width /* = 40 */, float height /* = 30 */, float nearZ /* = 1.0f */, float farZ /* = 1000.0f */)
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

	Matrix OrthoProjection2D(int width, int height)
	{
		float hw = 2.0f / width;
		float hh = -2.0f / height;
		return Matrix(hw, 0, 0, 0,
					  0, hh, 0, 0,
					  0, 0, 1, 0,
					  -1, 1, 0, 1);
	}

}