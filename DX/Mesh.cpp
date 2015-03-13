//////////////////////////////////////////////////////////////////////

#include "DX.h"

using namespace DX;

//////////////////////////////////////////////////////////////////////

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

void Scene::Unload()
{
	for(uint i = 0; i < mShader->ps.mNumTextures; ++i)
	{
		Texture * &t = mShader->ps.mTextures[i];
		if(t != null && t != mDefaultTexture.get())
		{
			Delete(t);
		}
	}
	for(uint i = 0; i < mShader->ps.mNumSamplers; ++i)
	{
		Sampler * &s = mShader->ps.mSamplers[i];
		if(s != null && s != mDefaultSampler.get())
		{
			Delete(s);
		}
	}

	mShader.reset();

	for(auto &mesh : mMeshes)
	{
		delete mesh.mIndexBuffer;
		delete mesh.mVertexBuffer;
	}
}

//////////////////////////////////////////////////////////////////////

HRESULT Scene::Load(tchar const *filename)
{
	using namespace Assimp;

#if defined(_DEBUG)
	DefaultLogger::create("", Logger::VERBOSE, aiDefaultLogStream_DEBUGGER);
#endif

	Importer importer;
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	uint options = aiProcess_Triangulate | aiProcess_SortByPType;
	importer.ReadFile(filename, options);
	aiScene const *scene = importer.GetScene();

#if defined(_DEBUG)
	DefaultLogger::kill();
#endif

	if(scene == null)
	{
		TRACE("Error loading %s: %s\n", filename, importer.GetErrorString());
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

	Shaders::Default::PS &ps = mShader->ps;

	auto l = ps.Light.Get();
	l->lightPos = Float3(0, -15, 20);
	l->ambientColor = Float3(0.1f, 0.1f, 0.1f);
	l->diffuseColor = Float3(0.8f, 0.8f, 0.8f);
	l->specColor = Float3(10, 10, 10);
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
			ps.tex1Sampler = new Sampler();
		}
	}

	// fill in default textures and samplers
	for(uint i = 0; i < ps.mNumTextures; ++i)
	{
		if(ps.mTextures[i] == null)
		{
			ps.mTextures[i] = mDefaultTexture.get();
		}
	}
	for(uint i = 0; i < ps.mNumSamplers; ++i)
	{
		if(ps.mSamplers[i] == null)
		{
			ps.mSamplers[i] = mDefaultSampler.get();
		}
	}

	for(uint m = 0; m < scene->mNumMeshes; ++m)
	{
		aiMesh const &mesh = *scene->mMeshes[m];

		Mesh &msh = mMeshes[m];

		mMeshes[m].mBase = 0;
		mMeshes[m].mCount = (uint16)mesh.mNumFaces * 3;

		// create index buffer
		{
			vector<uint16> indices;
			indices.reserve(mesh.mNumFaces * 3);
			uint x = 0;
			for(uint f = 0; f < mesh.mNumFaces; ++f)
			{
				aiFace const &face = mesh.mFaces[f];
				if(face.mNumIndices == 3)
				{
					indices.push_back(face.mIndices[2]);
					indices.push_back(face.mIndices[1]);
					indices.push_back(face.mIndices[0]);
				}
			}
			msh.mIndexBuffer = new IndexBuffer<uint16>((uint16)indices.size(), indices.data(), StaticUsage, NotCPUAccessible);
		}

		// create vertex buffer
		{
			using Vert = Shaders::Default::InputVertex;
			vector<Vert> verts;
			verts.resize(mesh.mNumVertices);
			for(uint v = 0; v < mesh.mNumVertices; ++v)
			{
				aiVector3D &src = mesh.mVertices[v];
				Vert &dst = verts[v];
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
			msh.mVertexBuffer = new VertexBuffer<Vert>((uint)verts.size(), verts.data(), StaticUsage, NotCPUAccessible);
		}
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
		mShader->vs.VertConstants.TransformMatrix = Transpose(transform);
		mShader->vs.VertConstants.ModelMatrix = modelMatrix;
		mShader->vs.VertConstants.Update(context);
		for(auto const m : node.mMeshes)
		{
			mShader->vs.SetVertexBuffers(context, 1, m->mVertexBuffer);
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
	mShader->ps.Camera.cameraPos = cameraPos;
	mShader->ps.Camera.Update(context);
	mShader->Activate(context);
	RenderNode(context, mRootNode, cameraMatrix, Transpose(modelMatrix));
}

//////////////////////////////////////////////////////////////////////

void Scene::CleanUp()
{
	mDefaultTexture.reset();
	mDefaultSampler.reset();
}

//////////////////////////////////////////////////////////////////////

