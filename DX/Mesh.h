//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Scene
	{
		//////////////////////////////////////////////////////////////////////

		struct Mesh
		{
			uint16								mBase;
			uint16								mCount;
			Shaders::Default::VertBuffer *		mVertexBuffer;
			IndexBuffer<uint16> *				mIndexBuffer;
		};

		//////////////////////////////////////////////////////////////////////

		struct Node: Aligned16
		{
			Matrix			mTransform;
			Node *			mParent;
			vector<Mesh *>	mMeshes;
			vector<Node>	mChildren;
		};

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(aiScene const *scene, ID3D11DeviceContext *context);
		void Render(ID3D11DeviceContext *context, Matrix cameraMatrix);

		void RenderNode(ID3D11DeviceContext *context, Node &node, Matrix const &transform);

		//////////////////////////////////////////////////////////////////////

		Ptr<Shaders::Default>				mShader;
		vector<Mesh>						mMeshes;
		Node								mRootNode;

		static Ptr<Texture>					mDefaultTexture;
		static Ptr<Sampler>					mDefaultSampler;
	};
}
