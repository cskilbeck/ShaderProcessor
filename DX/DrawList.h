//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX
{
	struct DrawList;

	struct VertexBuilderBase: TypelessBuffer
	{
		byte *mVertBase;
		byte *mCurrentVert;
		uint32 mVertexSize;

		VertexBuilderBase(uint32 vertexSize)
			: TypelessBuffer()
			, mCurrentVert(null)
			, mVertBase(null)
			, mVertexSize(vertexSize)
		{

		}

		uint Count() const
		{
			assert(mCurrentVert != null && mVertBase != null);
			return (uint)((mCurrentVert - mVertBase) / mVertexSize);
		}

		HRESULT Map(ID3D11DeviceContext *context)
		{
			DXR(TypelessBuffer::Map(context, mVertBase));
			mCurrentVert = mVertBase;
			return S_OK;
		}

		void UnMap(ID3D11DeviceContext *context)
		{
			TypelessBuffer::UnMap(context);
			mVertBase = null;
			mCurrentVert = null;
		}

		uint DataSize() const
		{
			return mVertexSize;
		}
	};

	template <typename T> struct VertexBuilder: VertexBuilderBase
	{
		VertexBuilder()
			: VertexBuilderBase(sizeof(T))
		{
		}

		HRESULT Create(uint numVerts)
		{
			DXR(TypelessBuffer::CreateBuffer(VertexBufferType, sizeof(T) * numVerts));
			return S_OK;
		}

		T &AddVertex()
		{
			T *p = (T *)mCurrentVert;
			mCurrentVert += sizeof(T);
			return *p;
		}

		VertexBuilder &AddVertex(T const &vertex)
		{
			AddVertex() = vertex;
			return *this;
		}
	};

	struct DrawList
	{
		DrawList();
		~DrawList();

		void SetShader(ID3D11DeviceContext *context, ShaderState *shader, VertexBuilderBase *vb);
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

		void End();

		// Remember to UnMap all your VertexBuilders before you call Execute...
		void Execute();

	private:

		template<typename T> T *Add();
		byte *AddData(byte const *data, uint size);
		void BeginDrawCall(uint32 topology);
		void SetConsts(ShaderType shaderType, byte *data, uint size, uint index);
		void UnMapCurrentVertexBuffer();

		byte *					mItemBuffer;			// base of Items buffer
		byte *					mItemPointer;			// current Item
		uint32					mVertexSize;			// size of a vert for current drawcall
		VertexBuilderBase *		mCurrentVertexBuffer;	// current VertexBuilder
		void *					mCurrentDrawCallItem;	// current DrawCall item
		ID3D11DeviceContext *	mContext;				// current Context
		ShaderState *			mCurrentShader;			// current Shader
	};

	//////////////////////////////////////////////////////////////////////

	template<typename T> inline T *DrawList::Add()
	{
		T *p = (T *)mItemPointer;
		mItemPointer += sizeof(T);
		p->mType = (uint)T::eType;
		return p;
	}

	//////////////////////////////////////////////////////////////////////

	template <typename T> inline void DrawList::SetConstantData(ShaderType shaderType, T &data, uint index)
	{
		SetConsts(shaderType, (byte *)&data, sizeof(T), index);
	}

}
