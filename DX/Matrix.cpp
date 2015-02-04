//////////////////////////////////////////////////////////////////////

#include "DX.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

Matrix IdentityMatrix(1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

//////////////////////////////////////////////////////////////////////

Vec4f GetYawPitchRollFromMatrix(Matrix const &m)
{
	MatrixArray const &a = MatrixAsArray(m);
	float roll = atan2(a[2][0], a[2][1]);
	float pitch = acos(a[2][2]);
	float yaw = -atan2(a[0][2], a[1][2]);
	return Vec4(yaw, pitch, roll);
}
