//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

extern CD3D11_DEPTH_STENCIL_DESC depthStencilDesc;
extern CD3D11_RASTERIZER_DESC rasterizerDesc;
extern CD3D11_BLEND_DESC blendDesc;

uint ScanMaterialOptions(tchar const *filename, string &file);
void OutputPragmaDocs();

//////////////////////////////////////////////////////////////////////

struct Semantic
{
	DXGI_FormatDescriptor *nativeFormat;
	StorageType type;
	int stream;
	int instances;
	bool instanced;

	bool Set(string const &type_name, string const &instances_str, string const &stream_str);
};

extern std::map<string, Semantic> semantics;

