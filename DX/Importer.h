#pragma once

namespace DX
{
	int LoadScene(tchar const *filename, aiScene const **scene);
	aiNode *FindNode(aiScene const *scene, char const *name);
}