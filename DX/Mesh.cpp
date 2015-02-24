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

	void CopyNodesWithMeshes(aiNode &node, Scene &scene, Scene::Node *targetParent, Matrix accTransform)
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
			Matrix t;
			CopyMatrix(t, &node.mTransformation);
			for(uint i = 0; i < 4; ++i)
			{
				TRACE("%f,%f,%f,%f\n", GetX(t.r[i]), GetY(t.r[i]), GetZ(t.r[i]), GetW(t.r[i]));
			}
			TRACE("\n");
			transform = t * accTransform;
		}
		// continue for all child nodes
		for(uint a = 0; a < node.mNumChildren; ++a)
		{
			CopyNodesWithMeshes(*node.mChildren[a], scene, parent, transform);
		}
	}
}

//////////////////////////////////////////////////////////////////////

HRESULT Scene::Create(aiScene const *scene, ID3D11DeviceContext *context)
{
	mRootNode.mTransform = IdentityMatrix;
	mRootNode.mParent = null;

	mShader.reset(new Shaders::Default());
	mMeshes.resize(scene->mNumMeshes);

	for(uint m = 0; m < scene->mNumMeshes; ++m)
	{
		aiMesh const &mesh = *scene->mMeshes[m];

		Mesh &msh = mMeshes[m];

		mMeshes[m].mBase = 0;
		mMeshes[m].mCount = (uint16)mesh.mNumFaces * 3;
		msh.mVertexBuffer = new VertexBuffer<Shaders::Default::InputVertex>(mesh.mNumVertices, null, DynamicUsage, Writeable);
		msh.mIndexBuffer = new IndexBuffer<uint16>((uint16)mesh.mNumFaces * 3, null, DynamicUsage, Writeable);

		uint16 *indices = msh.mIndexBuffer->Map(context);
		Shaders::Default::InputVertex *verts = msh.mVertexBuffer->Map(context);

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
			dst.Position.x = src.x;
			dst.Position.y = src.y;
			dst.Position.z = src.z;
			dst.Normal = { 1, 0, 0 };
			dst.TexCoord = { 0, 0 };
			dst.Color = 0xff000000;
		}

		msh.mIndexBuffer->UnMap(context);
		msh.mVertexBuffer->UnMap(context);
	}

	// count the nodes
	uint nodeCount = 0;
	ScanNodes(scene->mRootNode, [&nodeCount] (aiNode *node)
	{
		++nodeCount;
	});
	
	CopyNodesWithMeshes(*scene->mRootNode, *this, &mRootNode, IdentityMatrix);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////

void Scene::RenderNode(ID3D11DeviceContext *context, Scene::Node &node, Matrix const &transform)
{
	Matrix mat = transform * node.mTransform;
	if(!node.mMeshes.empty())
	{
		mShader->vs.VertConstants.TransformMatrix = Transpose(mat);
		mShader->vs.VertConstants.Commit(context);
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
		RenderNode(context, node, mat);
	}
}

//////////////////////////////////////////////////////////////////////

void Scene::Render(ID3D11DeviceContext *context, Matrix cameraMatrix)
{
	mShader->Activate(context);
	RenderNode(context, mRootNode, cameraMatrix);
}
