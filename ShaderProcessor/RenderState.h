//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

extern CD3D11_DEPTH_STENCIL_DESC depthStencilDesc;
extern CD3D11_RASTERIZER_DESC rasterizerDesc;
extern CD3D11_BLEND_DESC blendDesc;

uint ScanMaterialOptions(tchar const *filename, string &file);
void OutputPragmaDocs();



