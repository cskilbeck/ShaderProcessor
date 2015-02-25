//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct Scene: Aligned16
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

		struct Node
		{
			Matrix					mTransform;
			Node *					mParent;
			vector<Mesh *>			mMeshes;
			aligned_vector<Node>	mChildren;
		};

		//////////////////////////////////////////////////////////////////////

		HRESULT Create(tchar const *filename);

		void Render(ID3D11DeviceContext *context, Matrix &modelMatrix, Matrix &cameraMatrix, Vec4f cameraPos);
		void RenderNode(ID3D11DeviceContext *context, Node &node, Matrix const &transform, Matrix const &modelMatrix);

		//////////////////////////////////////////////////////////////////////

		Node								mRootNode;
		Ptr<Shaders::Default>				mShader;
		vector<Mesh>						mMeshes;

		static Ptr<Texture>					mDefaultTexture;
		static Ptr<Sampler>					mDefaultSampler;
	};
}
