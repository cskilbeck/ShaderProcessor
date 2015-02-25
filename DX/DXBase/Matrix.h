//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	using Matrix = DirectX::XMMATRIX;
	using MatrixArray = float[4][4];
	using Float4x4 = DX::Matrix;

	inline MatrixArray &MatrixAsArray(Matrix &m)
	{
		return *(MatrixArray *)&m;
	}

	inline MatrixArray const &MatrixAsArray(Matrix const &m)
	{
		return *(MatrixArray const *)&m;
	}

	extern Matrix IdentityMatrix;
	extern Matrix NegativeIdentityMatrix;

	//////////////////////////////////////////////////////////////////////

	Matrix		Transpose(Matrix const &m);
	Matrix		RotationMatrix(float yaw, float pitch, float roll);
	Matrix		RotationMatrix(Vec4f axis, float angle);
	Matrix		RotationMatrix(Vec4f yawPitchRoll);
	Matrix		TranslationMatrix(Vec4f translation);
	Vec4f		TransformPoint(Vec4f pos, Matrix const &m);
	Vec4f		GetPitchYawRollFromMatrix(Matrix const &m);
	Matrix		ViewMatrix(Vec4f target, Vec4f position, Vec4f up);
	Matrix		ViewMatrix(Vec4f position, float yaw, float pitch, float roll);
	Matrix		PerspectiveProjection(float fov = 0.5f, float aspectRatio = 4.0f / 3.0f, float nearZ = 1.0f, float farZ = 1000.0f);
	Matrix		OrthoProjection3D(float width, float height, float nearZ = 1.0f, float farZ = 1000.0f);
	Matrix		OrthoProjection2D(int width, int height);

	//////////////////////////////////////////////////////////////////////

	inline Matrix Transpose(Matrix const &m)
	{
		return DirectX::XMMatrixTranspose(m);
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix RotationMatrix(float yaw, float pitch, float roll)
	{
		return DirectX::XMMatrixRotationRollPitchYaw(yaw, pitch, roll);	// swap Y & Z...
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix RotationMatrix(Vec4f yawPitchRoll)
	{
		float yaw = GetX(yawPitchRoll);
		float pitch = GetY(yawPitchRoll);
		float roll = GetZ(yawPitchRoll);
		return DirectX::XMMatrixRotationRollPitchYaw(yaw, pitch, roll);	// swap Y & Z...
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix RotationMatrix(Vec4f axis, float angle)
	{
		return DirectX::XMMatrixRotationNormal(axis, angle);
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix TranslationMatrix(Vec4f translation)
	{
		return DirectX::XMMatrixTranslationFromVector(translation);
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix ScaleMatrix(Vec4f scale)
	{
		return DirectX::XMMatrixScalingFromVector(scale);
	}

	//////////////////////////////////////////////////////////////////////

	inline Vec4f TransformPoint(Vec4f pos, DX::Matrix const &m)
	{
		Vec4f r = SplatX(pos) * m.r[0] + SplatY(pos) * m.r[1] + SplatZ(pos) * m.r[2] + m.r[3];
		return r / SplatW(r);
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix const &CopyMatrix(void *f, DX::Matrix const &m)
	{
		DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4 *)f, m);
		return m;
	}

	//////////////////////////////////////////////////////////////////////

	inline Matrix &CopyMatrix(DX::Matrix &m, void const *f)
	{
		m = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)f);
		return m;
	}

	//////////////////////////////////////////////////////////////////////

	extern Pool<Matrix> MatrixPool;

	inline Matrix &MatrixAlloc()
	{
		return MatrixPool.Alloc();
	}

	inline void MatrixFree(Matrix &m)
	{
		MatrixPool.Free(m);
	}
}