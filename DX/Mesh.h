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
			Shaders::Default::VertBuffer		mVertexBuffer;
			IndexBuffer<uint16>					mIndexBuffer;
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

		HRESULT Load(tchar const *filename);
		void Unload();

		void Render(ID3D11DeviceContext *context, Matrix &modelMatrix, Matrix &cameraMatrix, Vec4f cameraPos);
		void RenderNode(ID3D11DeviceContext *context, Node &node, Matrix const &transform, Matrix const &modelMatrix);

		//////////////////////////////////////////////////////////////////////

		Node								mRootNode;
		Shaders::Default					mShader;
		vector<Mesh>						mMeshes;

		static void CleanUp();

		static Texture						mDefaultTexture;
		static Sampler						mDefaultSampler;
	};
}
