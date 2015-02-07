//////////////////////////////////////////////////////////////////////

#include "DX.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

Matrix IdentityMatrix(1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0,
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
