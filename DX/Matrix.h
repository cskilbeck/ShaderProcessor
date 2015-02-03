//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

typedef DirectX::XMMATRIX Matrix;
typedef float MatrixArray[4][4];

extern Matrix IdentityMatrix;

//////////////////////////////////////////////////////////////////////

Matrix		TransposeMatrix(Matrix const &m);
Matrix		RotationMatrix(float yaw, float pitch, float roll);
Matrix		RotationMatrix(Vec4f axis, float angle);
Matrix		TranslationMatrix(Vec4f translation);
Vec4f		TransformPoint(Vec4f pos, Matrix const &m);
Vec4f		GetYawPitchRollFromMatrix(Matrix const &m);

//////////////////////////////////////////////////////////////////////

inline Matrix TransposeMatrix(Matrix const &m)
{
	return XMMatrixTranspose(m);
}

//////////////////////////////////////////////////////////////////////

inline Matrix RotationMatrix(float yaw, float pitch, float roll)
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
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

inline Vec4f TransformPoint(Vec4f pos, Matrix const &m)
{
	Vec4f r = SplatX(pos) * m.r[0] + SplatY(pos) * m.r[1] + SplatZ(pos) * m.r[2] + m.r[3];
	return r / SplatW(r);
}

//////////////////////////////////////////////////////////////////////

