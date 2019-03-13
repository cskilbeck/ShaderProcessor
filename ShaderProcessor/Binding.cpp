//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

namespace {
//////////////////////////////////////////////////////////////////////

using BindingInfoMap_t = std::map<uint, BindingInfo>;

BindingInfoMap_t BindingInfoMap = { { D3D_SIT_CBUFFER, { true, BindingInfo::Type::ConstantBuffer, "CBUFFER", "ConstantBuffer" } },
                                    { D3D_SIT_TBUFFER, { false, BindingInfo::Type::Resource, "TBUFFER", "TextureBuffer" } },
                                    { D3D_SIT_TEXTURE, { false, BindingInfo::Type::Resource, "TEXTURE", "Texture" } },
                                    { D3D_SIT_SAMPLER, { false, BindingInfo::Type::SamplerState, "SAMPLER", "Sampler" } },
                                    { D3D_SIT_UAV_RWTYPED, { false, BindingInfo::Type::Resource, "UAV_RWTYPED", "RW_UAVTyped" } },
                                    { D3D_SIT_STRUCTURED, { true, BindingInfo::Type::Resource, "STRUCTURED", "StructuredInput" } },
                                    { D3D_SIT_UAV_RWSTRUCTURED, { true, BindingInfo::Type::Resource, "UAV_RWSTRUCTURED", "RW_UAVStructured" } },
                                    { D3D_SIT_BYTEADDRESS, { false, BindingInfo::Type::Resource, "BYTEADDRESS", "ByteAddress" } },
                                    { D3D_SIT_UAV_RWBYTEADDRESS, { false, BindingInfo::Type::Resource, "UAV_RWBYTEADDRESS", "RW_ByteAddress" } },
                                    { D3D_SIT_UAV_APPEND_STRUCTURED, { true, BindingInfo::Type::Resource, "UAV_APPEND_STRUCTURED", "AppendStructured" } },
                                    { D3D_SIT_UAV_CONSUME_STRUCTURED, { true, BindingInfo::Type::Resource, "UAV_CONSUME_STRUCTURED", "ConsumeStructured" } },
                                    { D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER,
                                      { true, BindingInfo::Type::Resource, "UAV_RWSTRUCTURED_WITH_COUNTER", "RW_AUVStructuredWithCounter" } } };

}    // namespace

//////////////////////////////////////////////////////////////////////

BindingInfo *GetBindingInfo(D3D_SHADER_INPUT_TYPE type)
{
    auto i = BindingInfoMap.find(type);
    return (i != BindingInfoMap.end()) ? &i->second : null;
}

//////////////////////////////////////////////////////////////////////

Binding::Binding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc) : mShader(s), mDesc(desc), definition(null)
{
    BindingInfo *info = GetBindingInfo(desc.Type);
    if(info != null) {
        s->AddBinding(this);
    } else {
        emit_warning("Unknown binding type: %d", desc.Type);
    }
}

//////////////////////////////////////////////////////////////////////

string Binding::TypeName()
{
    if(definition != null) {
        return definition->Name;
    } else {
        return "??";
    }
}

//////////////////////////////////////////////////////////////////////

ConstantBufferBinding::ConstantBufferBinding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc) : Binding(s, desc)
{
    // Find the definition, previously created...
    auto d = s->mDefinitionIDs.find(this->Name());
    if(d != s->mDefinitionIDs.end()) {
        TypeDefinition *def = s->mDefinitions[d->second];
        def->mBinding = this;
        definition = def;
    }
}
