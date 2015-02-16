//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DrawList
	{
		DrawList();
		~DrawList();

		template<typename T, typename U> void Reset(ID3D11DeviceContext *context, T *shader, U *vertbuffer, Material &material);
		void SetShader(ShaderState *shader, TypelessBuffer *vb, uint vertexSize);
		void SetMaterial(Material &m);
		void SetPSTexture(Texture &t, uint index = 0);
		void SetPSSampler(Sampler &s, uint index = 0);
		template <typename T> void SetVSConstantData(T &data, uint index);
		template <typename T> void SetGSConstantData(T &data, uint index);
		template <typename T> void SetPSConstantData(T &data, uint index);
		void BeginPointList();
		void BeginTriangleList();
		void BeginTriangleStrip();
		void BeginLineList();
		void BeginLineStrip();
		template <typename T> void AddVertex(T const &vertex);
		template <typename T> T &GetVertex();
		void End();
		void Execute();

	private:

		template<typename T> T *Add();
		byte *AddData(byte const *data, uint size);
		void BeginDrawCall(uint32 topology);

		void SetVSConsts(byte *data, uint size, uint index);
		void SetGSConsts(byte *data, uint size, uint index);
		void SetPSConsts(byte *data, uint size, uint index);

		byte *mItemBuffer;
		byte *mItemPointer;
		uint32 mVertexSize;

		byte *mVertZero;
		byte *mVertPointer;
		byte *mVertBase;
		
		void *mCurrentDrawCallItem;
		ID3D11DeviceContext *mContext;
		TypelessBuffer *mCurrentVertexBuffer;
		ShaderState *mCurrentShader;
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T, typename U> inline void DrawList::Reset(ID3D11DeviceContext *context, T *shader, U *vertbuffer, Material &material)
	{
		if(mCurrentVertexBuffer != null && mVertBase != null)
		{
			// Yank
			mCurrentVertexBuffer->UnMap(mContext);
		}
		mCurrentVertexBuffer = vertbuffer;
		mContext = context;
		mItemPointer = mItemBuffer;
		mCurrentDrawCallItem = null;
		SetShader(shader, vertbuffer, sizeof(T::InputVertex));
		SetMaterial(material);
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

	template <typename T> inline T &DrawList::GetVertex()
	{
		assert(sizeof(T) == mVertexSize);
		T *p = (T *)mVertPointer;
		mVertPointer += sizeof(T);
		return *p;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetVSConstantData(T &data, uint index)
	{
		SetVSConsts((byte *)&data, sizeof(T), index);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetGSConstantData(T &data, uint index)
	{
		SetGSConsts((byte *)&data, sizeof(T), index);
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetPSConstantData(T &data, uint index)
	{
		SetPSConsts((byte *)&data, sizeof(T), index);
	}
}
