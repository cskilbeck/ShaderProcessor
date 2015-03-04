
#pragma once

#include "targetver.h"

#include "DXBase\DXBase.h"

#include "Error.h"
#include "printer.h"
#include "TypeDefinition.h"
#include "Binding.h"
#include "TBufferBinding.h"
#include "TextureBinding.h"
#include "SamplerBinding.h"
#include "HLSLShader.h"
#include "optionparser.h"
#include "Args.h"
#include "RenderState.h"

#ifndef _MBCS
#error "UNICODE not supported in this tool!"
#endif