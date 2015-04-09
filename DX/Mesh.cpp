//////////////////////////////////////////////////////////////////////

#include "DX.h"
#include "assimp/include/assimp/IOSystem.hpp"
#include "assimp/include/assimp/IOStream.hpp"

using namespace DX;

//////////////////////////////////////////////////////////////////////

namespace
{
	class MyIOStream : public Assimp::IOStream
	{
	public:
		~MyIOStream()
		{
			Delete(file);
		}

		size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
		{
			uint64 g;
			if(file->Read(pvBuffer, pSize * pCount, &g) != S_OK)
			{
				return 0;
			}
			return g / pSize;
		}

		size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override
		{
			return 0;
		}

		aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
		{
			return aiReturn_FAILURE;
		}

		size_t Tell() const override
		{
			return 0;
		}

		size_t FileSize() const override
		{
			uint64 size;
			if(file->GetSize(size) == S_OK)
			{
				return size;
			}
			return 0;
		}

		void Flush() override
		{
		}
	private:
		friend class MyIOSystem;

		MyIOStream()
			: file(null)
		{
		}

		FileBase *file;
	};

	class MyIOSystem : public Assimp::IOSystem
	{
	public:
		MyIOSystem()
		{
		}

		~MyIOSystem()
		{
		}

		void Close(Assimp::IOStream *stream) override
		{
			MyIOStream *s = (MyIOStream *)stream;
			FileBase * &f = s->file;
			Delete(f);
		}

		bool ComparePaths(const char *one, const char *second) const override
		{
			return _stricmp(one, second) == 0;
		}

		bool ComparePaths(std::string const &one, std::string const &two) const
		{
			return ComparePaths(one.c_str(), two.c_str());
		}

		bool Exists(char const *pFile) const override
		{
			tstring s = DX::TStringFromString(pFile);
			return DX::AssetManager::FileExists(s.c_str());
		}

		bool Exists(string const &str) const
		{
			return Exists(str.c_str());
		}

		char getOsSeparator() const override
		{
			return '/';
		}

		Assimp::IOStream *Open(const char *f, const char *mode = "rb") override
		{
			if(_stricmp(mode, "rb") != 0)
			{
				return null;
			}
			MyIOStream *s = new MyIOStream();
			if(AssetManager::Open(f, &s->file) != S_OK)
			{
				Delete(s);
				return null;
			}
			return s;
		}

		Assimp::IOStream *Open(string const &f, string const &mode)
		{
			return Open(f.c_str(), mode.c_str());
		}
	};

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

Texture Scene::mDefaultTexture;
Sampler Scene::mDefaultSampler;

//////////////////////////////////////////////////////////////////////

void Scene::Unload()
{
	for(uint i = 0; i < mShader.ps.mNumTextures; ++i)
	{
		Texture * &t = mShader.ps.mTextures[i];
		if(t != null && t != &mDefaultTexture)
		{
			Delete(t);
		}
	}
	for(uint i = 0; i < mShader.ps.mNumSamplers; ++i)
	{
		Sampler * &s = mShader.ps.mSamplers[i];
		if(s != null && s != &mDefaultSampler)
		{
			Delete(s);
		}
	}

	mShader.Release();

	for(auto &mesh : mMeshes)
	{
		mesh.mIndexBuffer.Release();
		mesh.mVertexBuffer.Release();
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
	importer.SetIOHandler(new MyIOSystem());
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

	mShader.Create();
	mMeshes.resize(scene->mNumMeshes);

	// Need to free these somehow...
	if(!mDefaultTexture.IsValid())
	{
		Texture::CreateGrid(mDefaultTexture, 64, 64, 8, 8, Color::DarkGray, Color::LightGray);
		mDefaultSampler.Create();
	}

	Shaders::Default::PS &ps = mShader.ps;

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
			ps.picTexture = new Texture();
			ps.picTexture->Load(texturePath.c_str());
			ps.tex1Sampler = new Sampler();
			ps.tex1Sampler->Create();
		}
	}

	// fill in default textures and samplers
	for(uint i = 0; i < ps.mNumTextures; ++i)
	{
		if(ps.mTextures[i] == null)
		{
			ps.mTextures[i] = &mDefaultTexture;
		}
	}
	for(uint i = 0; i < ps.mNumSamplers; ++i)
	{
		if(ps.mSamplers[i] == null)
		{
			ps.mSamplers[i] = &mDefaultSampler;
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
			msh.mIndexBuffer.Create((uint16)indices.size(), indices.data(), StaticUsage, NotCPUAccessible);
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
			msh.mVertexBuffer.Create((uint)verts.size(), verts.data(), StaticUsage, NotCPUAccessible);
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
		mShader.vs.VertConstants.TransformMatrix = Transpose(transform);
		mShader.vs.VertConstants.ModelMatrix = modelMatrix;
		mShader.vs.VertConstants.Update(context);
		for(auto const m : node.mMeshes)
		{
			mShader.vs.SetVertexBuffers(context, 1, &m->mVertexBuffer);
			m->mIndexBuffer.Activate(context);
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
	mShader.ps.Camera.cameraPos = cameraPos;
	mShader.ps.Camera.Update(context);
	mShader.Activate(context);
	RenderNode(context, mRootNode, cameraMatrix, Transpose(modelMatrix));
}

//////////////////////////////////////////////////////////////////////

void Scene::CleanUp()
{
	mDefaultTexture.Release();
	mDefaultSampler.Release();
}

//////////////////////////////////////////////////////////////////////

