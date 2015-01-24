//////////////////////////////////////////////////////////////////////
// detect ps/vs etc and make it derive from the right thing
// support everything (Buffers<> especially)
// fix texturebuffers by doing the '3 types' of thing split
// deal with anonymous cbuffers and tbuffers (index instead of name)
// test alignment/padding etc
// deal with Buffers of structs (no padding)

#include "stdafx.h"

using namespace DirectX;

//////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	int rc = 0;
	if(D3D::Open())
	{
		FileResource f(argv[1]);
		if(f.IsValid())
		{
			tstring extension(GetExtension(argv[1]));
			if(_tcsicmp(TEXT(".hlsl"), extension.c_str()) == 0)
			{
				uint flags = 0;
				Resource r;
				tstring output = SetExtension(argv[1], TEXT("cso"));
				ID3DBlob *compiledShader;
				ID3DBlob *errors;
				if(D3DCompileFromFile(argv[1], null, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", flags, 0, &compiledShader, &errors) == S_OK)
				{
					::Shader s(GetFilename(argv[1]));
					s.Create(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize());
				}
				else
				{
					printf("%s\n", errors->GetBufferPointer());
					rc = 1;
				}
			}
			else if(_tcsicmp(TEXT(".cso"), extension.c_str()) == 0)
			{
				::Shader s(GetFilename(argv[1]));
				s.Create(f, f.Size());
			}
		}
	}
	D3D::Close();
	return rc;
}
