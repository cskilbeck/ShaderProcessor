//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////

struct TextureBuffer : Binding
{
    TextureBuffer(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC desc) : Binding(s, desc)
    {
    }

    ~TextureBuffer() override
    {
    }

    void StaticsOutput() override
    {
    }

    void MemberOutput() override
    {
        printf("\t\tTextureBuffer *%s;\n", mDesc.Name);
    }

    void ConstructorOutput() override
    {
        printf("%s(null)\n", mDesc.Name);
    }
};
