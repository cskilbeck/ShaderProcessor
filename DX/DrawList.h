//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DrawList
	{
		DrawList();
		~DrawList();

		template<typename T> void Reset(ID3D11DeviceContext *context, ShaderState *shader, T *vertbuffer);
		void SetTexture(ShaderType shaderType, Texture &t, uint index = 0);
		void SetSampler(ShaderType shaderType, Sampler &s, uint index = 0);
		void SetScissorRect(Rect2D const &rect);
		void ResetScissorRect();
		template <typename T> void SetConstantData(ShaderType shaderType, T &data, uint index);
		void BeginPointList();
		void BeginTriangleList();
		void BeginTriangleStrip();
		void BeginLineList();
		void BeginLineStrip();
		template <typename T> void AddVertex(T const &vertex);
		template <typename T> T &AddVertex();
		int End();	// returns the # of verts which were submitted in the drawcall (which is no longer current)
		void Execute();

		Texture *GetCurrentTexture(uint index = 0);
		bool IsDrawCallInProgress() const;

	private:

		template<typename T> T *Add();
		byte *AddData(byte const *data, uint size);
		void BeginDrawCall(uint32 topology);
		void SetShader(ID3D11DeviceContext *context, ShaderState *shader, TypelessBuffer *vb, uint vertexSize);
		void SetConsts(ShaderType shaderType, byte *data, uint size, uint index);
		void UnMapCurrentVertexBuffer();

		byte *mItemBuffer;
		byte *mItemPointer;
		uint32 mVertexSize;

		byte *mVertZero;
		byte *mVertPointer;
		byte *mVertBase;

		Texture *mTextures[8];
		
		void *mCurrentDrawCallItem;
		ID3D11DeviceContext *mContext;
		TypelessBuffer *mCurrentVertexBuffer;
		ShaderState *mCurrentShader;
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline void DrawList::Reset(ID3D11DeviceContext *context, ShaderState *shader, T *vertbuffer)
	{
		SetShader(context, shader, vertbuffer, T::SizeOf());
	}

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline T *DrawList::Add()
	{
		T *p = (T *)mItemPointer;
		mItemPointer += sizeof(T);
		p->mType = (uint)T::eType;
		return p;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::AddVertex(T const &vertex)
	{
		assert(sizeof(T) == mVertexSize);
		*((T *)mVertPointer) = vertex;
		mVertPointer += sizeof(T);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline T &DrawList::AddVertex()
	{
		assert(sizeof(T) == mVertexSize);
		T *p = (T *)mVertPointer;
		mVertPointer += sizeof(T);
		return *p;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetConstantData(ShaderType shaderType, T &data, uint index)
	{
		SetConsts(shaderType, (byte *)&data, sizeof(T), index);
	}

}
