//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	if(D3D::Open())
	{
		FileResource f(argv[1]);
		if(f.IsValid())
		{
			Shader s(GetFilename(argv[1]));
			s.Create(f, f.Size());
		}
	}
	D3D::Close();
	getchar();
	return 0;
}
