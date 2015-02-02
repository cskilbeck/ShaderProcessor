//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////

namespace
{
	//////////////////////////////////////////////////////////////////////

	using ISMap = std::map < int, string >;

	//////////////////////////////////////////////////////////////////////

	char const *BindingTypeNames[BindingInfo::Type::NumBindingTypes] =
	{
		"SamplerState",
		"Resource",
		"ConstantBuffer"
	};
		
	//////////////////////////////////////////////////////////////////////

	string const &GetFrom(ISMap const &map, int x)
	{
		static string const unknown("");
		auto i = map.find(x);
		return (i != map.end()) ? i->second : unknown;
	}

	//////////////////////////////////////////////////////////////////////

	ISMap constant_buffer_type_names =
	{
		{ D3D_CT_CBUFFER, "ConstantBuffer" },
		{ D3D_CT_TBUFFER, "TextureBuffer" },
		{ D3D_CT_INTERFACE_POINTERS, "InterfacePointers" },
		{ D3D_CT_RESOURCE_BIND_INFO, "ResourceBindingInfo" }
	};

	//////////////////////////////////////////////////////////////////////

	ISMap shader_input_type_names =
	{
		{ D3D_SIT_CBUFFER, "ConstantBuffer" },										// has 'Constant Buffer'	// ConstantBuffer
		{ D3D_SIT_TBUFFER, "TextureBuffer" },										// has 'Constant Buffer'	// Resource
		{ D3D_SIT_TEXTURE, "Texture" },												//							// Resource
		{ D3D_SIT_SAMPLER, "SamplerState" },										//							// SamplerState
		{ D3D_SIT_UAV_RWTYPED, "RW_UAVTyped" },										//							// Resource				// D3D11_BIND_UNORDERED_ACCESS
		{ D3D_SIT_STRUCTURED, "StructuredInput" },									// has 'Constant Buffer'	// Resource
		{ D3D_SIT_UAV_RWSTRUCTURED, "RW_UAVStructured" },														// Resource				// D3D11_BIND_UNORDERED_ACCESS
		{ D3D_SIT_BYTEADDRESS, "ByteAddress" },																	// Resource
		{ D3D_SIT_UAV_RWBYTEADDRESS, "RW_ByteAddress" },														// Resource				// D3D11_BIND_UNORDERED_ACCESS
		{ D3D_SIT_UAV_APPEND_STRUCTURED, "AppendStructured" },						// has 'Constant Buffer'	// Resource
		{ D3D_SIT_UAV_CONSUME_STRUCTURED, "ConsumeStructured" },					// has 'Constant Buffer'	// Resource
		{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, "RW_AUVStructuredWithCounter" }	// has 'Constant Buffer'	// Resource				// D3D11_BIND_UNORDERED_ACCESS
	};

	//////////////////////////////////////////////////////////////////////

	BindingInfoMap_t BindingInfoMap =
	{
		{ D3D_SIT_CBUFFER, { true, BindingInfo::Type::ConstantBuffer, "CBUFFER", "ConstantBuffer" } },
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
		{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, { true, BindingInfo::Type::Resource, "UAV_RWSTRUCTURED_WITH_COUNTER", "RW_AUVStructuredWithCounter" } }
	};
}

//////////////////////////////////////////////////////////////////////

Binding::ResourceBindingList_t Binding::ResourceBindingList[BindingInfo::Type::NumBindingTypes];

BindingInfo *GetBindingInfo(D3D_SHADER_INPUT_TYPE type)
{
	auto i = BindingInfoMap.find(type);
	return (i != BindingInfoMap.end()) ? &i->second : null;
}

char const *GetBindingTypeName(BindingInfo::Type bindingType)
{
	return BindingTypeNames[bindingType];
}

//////////////////////////////////////////////////////////////////////

Binding::Binding(HLSLShader *s, D3D11_SHADER_INPUT_BIND_DESC &desc)
	: mShader(s)
	, mDesc(desc)
	, definition(null)
{
	BindingInfo *info = GetBindingInfo(desc.Type);
	if(info != null)
	{
		if(info->NeedsDefinition)
		{
			// find the definition in the shader
			auto j = s->mDefinitionIDs.find(desc.Name);
			if(j != s->mDefinitionIDs.end())
			{
				definition = s->mDefinitions[j->second];
			}
			else
			{
				TRACE("ERROR! Where's my TypeDefinition!? I need one...\n");
			}
		}
		ResourceBindingList[info->BindingType].push_back(this);
	}
	else
	{
		TRACE("ERROR! Unknown binding type...\n");
	}
}

//////////////////////////////////////////////////////////////////////

string Binding::TypeName()
{
	if(definition != null)
	{
		return definition->Name;
	}
	else
	{
		return "??";
	}
}

//////////////////////////////////////////////////////////////////////

void Binding::ClearAllBindings()
{
	for(uint i = 0; i < BindingInfo::Type::NumBindingTypes; ++i)
	{
		ResourceBindingList[i].clear();
	}
}

//////////////////////////////////////////////////////////////////////

void Binding::ShowAllBindings()
{
	for(uint i = 0; i < BindingInfo::Type::NumBindingTypes; ++i)
	{
		ResourceBindingList_t &l = ResourceBindingList[i];

		TRACE("%s bindings:\n", BindingTypeNames[i]);

		for(auto j = l.begin(); j != l.end(); ++j)
		{
			Binding *b = *j;
			BindingInfo *info = GetBindingInfo(b->mDesc.Type);
			assert(info != null);
			TRACE("  %d:%s %s", b->mDesc.BindPoint, info->Name, b->Name());
			if(b->definition != null)
			{
				TRACE(" (Definition: %s)", b->definition->Name);
			}
			TRACE("\n");
		}
	}
}
