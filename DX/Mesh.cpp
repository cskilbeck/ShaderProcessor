//////////////////////////////////////////////////////////////////////

#include "DX.h"

using namespace DX;

//////////////////////////////////////////////////////////////////////

#define final_assert(x) { if(!(x)) DebugBreak(); }

namespace
{
	void ScanNodes(aiNode *node, std::function<void(aiNode *)> callback)
	{
		callback(node);
		for(uint i = 0; i < node->mNumChildren; ++i)
		{
			ScanNodes(node->mChildren[i], callback);
		}
	}

	void CopyNodesWithMeshes(Matrix &accTransform, aiNode &node, Scene &scene, Scene::Node *targetParent)
	{
		Scene::Node *parent;
		Matrix transform;
		// if node has meshes, create a new Scene::Node for it
		if(node.mNumMeshes > 0)
		{
			Scene::Node newObject;
			newObject.mMeshes.resize(node.mNumMeshes);
			newObject.mParent = null;
			newObject.mTransform = IdentityMatrix;

			// copy the meshes
			for(uint i = 0; i < node.mNumMeshes; ++i)
			{
				newObject.mMeshes[i] = &scene.mMeshes[node.mMeshes[i]];
			}

			size_t n = targetParent->mChildren.size();
			targetParent->mChildren.push_back(newObject);
			parent = &targetParent->mChildren[n];
			newObject.mParent = parent;
			transform = IdentityMatrix;
		}
		else
		{
			// if no meshes, skip the node, but keep its transformation
			parent = targetParent;
 			transform = XMLoadFloat4x4((const DirectX::XMFLOAT4X4 *)&node.mTransformation) * accTransform;
		}
		// continue for all child nodes
		for(uint a = 0; a < node.mNumChildren; ++a)
		{
			CopyNodesWithMeshes(transform, *node.mChildren[a], scene, parent);
		}
	}
}

//////////////////////////////////////////////////////////////////////

Ptr<Texture> Scene::mDefaultTexture;
Ptr<Sampler> Scene::mDefaultSampler;

//////////////////////////////////////////////////////////////////////

HRESULT Scene::Create(tchar const *filename)
{
	using namespace Assimp;

	DefaultLogger::create("", Logger::VERBOSE, aiDefaultLogStream_DEBUGGER);
	Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	uint options = aiProcess_Triangulate | aiProcess_SortByPType;
	importer.ReadFile(filename, options);
	aiScene const *scene = importer.GetScene();
	DefaultLogger::kill();

	if(scene == null)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	mRootNode.mTransform = IdentityMatrix;
	mRootNode.mParent = null;

	mShader.reset(new Shaders::Default());
	mMeshes.resize(scene->mNumMeshes);

	// Need to free these somehow...
	if(!mDefaultTexture)
	{
		mDefaultTexture.reset(Texture::Grid(64, 64, 8, 8, Color::DarkGray, Color::LightGray));
		mDefaultSampler.reset(new Sampler());
	}

	Shaders::Default ::PS &ps = mShader->ps;
	ps.picTexture = mDefaultTexture.get();
	ps.tex1Sampler = mDefaultSampler.get();

	auto l = ps.Light.Get();
	l->lightPos = Float3(0, -45, 0);
	l->ambientColor = Float3(0.5f, 0.5f, 0.5f);
	l->diffuseColor = Float3(0.5f, 0.5f, 0.5f);
	l->specColor = Float3(1, 1, 1);
	l.Release();

	// Material properties
	for(uint m = 0; m < scene->mNumMaterials; ++m)
	{
		aiMaterial const &mat = *scene->mMaterials[m];
		uint diffuseTextures = mat.GetTextureCount(aiTextureType_DIFFUSE);
		if(diffuseTextures > 0)
		{
			aiString path;
			mat.GetTexture(aiTextureType_DIFFUSE, 0, &path);
			string texture(path.C_Str());
			std::replace(texture.begin(), texture.end(), '/', '\\');
			texture = ReplaceAll(texture, string(".\\"), string(""));

			// can't handle TGA, look for a .PNG
			if(_tcsicmp(GetExtension(path.C_Str()).c_str(), ".tga") == 0)
			{
				texture = SetExtension(texture.c_str(), ".png");
			}
			tstring texturePath = GetPath(filename) + TStringFromString(texture);
			ps.picTexture = new Texture(texturePath.c_str());
		}
	}

	for(uint m = 0; m < scene->mNumMeshes; ++m)
	{
		aiMesh const &mesh = *scene->mMeshes[m];

		Mesh &msh = mMeshes[m];

		mMeshes[m].mBase = 0;
		mMeshes[m].mCount = (uint16)mesh.mNumFaces * 3;
		msh.mVertexBuffer = new VertexBuffer<Shaders::Default::InputVertex>(mesh.mNumVertices, null, DynamicUsage, Writeable);
		msh.mIndexBuffer = new IndexBuffer<uint16>((uint16)mesh.mNumFaces * 3, null, DynamicUsage, Writeable);

		uint16 *indices = msh.mIndexBuffer->Map();
		Shaders::Default::InputVertex *verts = msh.mVertexBuffer->Map();

		uint x = 0;
		for(uint f = 0; f < mesh.mNumFaces; ++f)
		{
			aiFace const &face = mesh.mFaces[f];
			if(face.mNumIndices == 3)
			{
				indices[x++] = face.mIndices[2];
				indices[x++] = face.mIndices[1];
				indices[x++] = face.mIndices[0];
			}
		}

		for(uint v = 0; v < mesh.mNumVertices; ++v)
		{
			aiVector3D &src = mesh.mVertices[v];
			Shaders::Default::InputVertex &dst = verts[v];
			dst.Position = { src.x, src.y, src.z };
			if(mesh.HasNormals())
			{
				dst.Normal = { mesh.mNormals[v].x, mesh.mNormals[v].y, mesh.mNormals[v].z };
			}
			else
			{
				dst.Normal = { 1, 0, 0 };
			}
			if(mesh.HasTextureCoords(0))
			{
				dst.TexCoord = { mesh.mTextureCoords[0][v].x, mesh.mTextureCoords[0][v].y };
			}
			else
			{
				dst.TexCoord = { (float)(v & 1), (float)(1 - (v & 1)) };
			}
			if(mesh.HasVertexColors(0))
			{
				dst.Color = Color((float *)&mesh.mColors[0]);
			}
			else
			{
				dst.Color = 0xffffffff;
			}
		}
		msh.mIndexBuffer->UnMap();
		msh.mVertexBuffer->UnMap();
	}

	CopyNodesWithMeshes(IdentityMatrix, *scene->mRootNode, *this, &mRootNode);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Scene::RenderNode(ID3D11DeviceContext *context, Scene::Node &node, Matrix const &transform, Matrix const &modelMatrix)
{
	// use a bool which tells whether any of this node's children have anything to render
	// if it's false, return;
	// else:
	if(!node.mMeshes.empty())
	{
		auto vc = mShader->vs.VertConstants.Get();
		vc->TransformMatrix = Transpose(transform);
		vc->ModelMatrix = modelMatrix;
		vc.Release();
		for(auto const m : node.mMeshes)
		{
			m->mVertexBuffer->Activate(context);
			m->mIndexBuffer->Activate(context);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(m->mCount, m->mBase, 0);
		}
	}
	for(auto &node: node.mChildren)
	{
		Matrix mat = transform * node.mTransform;
		RenderNode(context, node, mat, modelMatrix);
	}
}

//////////////////////////////////////////////////////////////////////

void Scene::Render(ID3D11DeviceContext *context, Matrix &modelMatrix, Matrix &cameraMatrix, Vec4f cameraPos)
{
	mShader->ps.Camera.Get()->cameraPos = cameraPos;
	mShader->Activate(context);
	RenderNode(context, mRootNode, cameraMatrix, Transpose(modelMatrix));
}
