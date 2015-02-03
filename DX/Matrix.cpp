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
	MatrixArray &a = (MatrixArray &)*(&m);
	return Vec4(asinf(a[1][0]), atan2f(-a[1][2], a[1][1]), atan2f(-a[2][0], a[0][0]));
}
