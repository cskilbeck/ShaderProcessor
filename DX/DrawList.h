//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

namespace DX {
struct DrawList;

struct VertexBuilderBase : TypelessBuffer
{
    byte *mVertBase;
    byte *mCurrentVert;
    uint32 mVertexSize;

    VertexBuilderBase(uint32 vertexSize) : TypelessBuffer(), mCurrentVert(null), mVertBase(null), mVertexSize(vertexSize)
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

template <typename T> struct VertexBuilder : VertexBuilderBase
{
    VertexBuilder() : VertexBuilderBase(sizeof(T))
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
    void SetViewport(Rect2D const &rect);
    void ResetScissorRect();
    template <typename T> void SetConstantData(ShaderType shaderType, TypelessBuffer *buffer, T &data);
    template <typename T> void SetConstantData(ShaderType shaderType, T &data, uint index);
    template <typename T> void SetConstantData(ShaderType shaderType, T &data);

    void Begin(uint32 topology);
    void BeginPointList();
    void BeginTriangleList();
    void BeginTriangleStrip();
    void BeginLineList();
    void BeginLineStrip();
    void End();

    // Remember to UnMap all your VertexBuilders before you call Execute...
    void Execute();

private:
    template <typename T> T *Add();
    byte *AddData(byte const *data, uint size);
    void SetConsts(ShaderType shaderType, byte *data, uint size, uint index);
    void SetConsts(ShaderType shaderType, TypelessBuffer *buffer, byte *data, uint32 size);
    void UnMapCurrentVertexBuffer();

    byte *mItemBuffer;                          // base of Items buffer
    byte *mItemPointer;                         // current Item
    VertexBuilderBase *mCurrentVertexBuffer;    // current VertexBuilder
    void *mCurrentDrawCallItem;                 // current DrawCall item
    ID3D11DeviceContext *mContext;              // current Context
    ShaderState *mCurrentShader;                // current Shader
    uint32 mVertexSize;                         // size of a vert for current drawcall
    uint32 mCommandBufferSize;                  // room for the commands
};

//////////////////////////////////////////////////////////////////////

template <typename T> inline T *DrawList::Add()
{
    uint current = (uint)(mItemPointer - mItemBuffer) + sizeof(T);
    if(current >= mCommandBufferSize) {
        assert(false);
    }
    T *p = (T *)mItemPointer;
    mItemPointer += sizeof(T);
    p->mType = (uint)T::eType;
    return p;
}

//////////////////////////////////////////////////////////////////////
// set constant data regardless of shaderstate association

template <typename T> inline void DrawList::SetConstantData(ShaderType shaderType, TypelessBuffer *buffer, T &data)
{
    SetConsts(Vertex, buffer, (byte *)&data, sizeof(data));
}

//////////////////////////////////////////////////////////////////////

template <typename T> inline void DrawList::SetConstantData(ShaderType shaderType, T &data, uint index)
{
    SetConsts(shaderType, (byte *)&data, sizeof(T), index);
}

template <typename T> inline void DrawList::SetConstantData(ShaderType shaderType, T &data)
{
    SetConsts(shaderType, (byte *)&data, sizeof(T), T::Index);
}
}    // namespace DX
