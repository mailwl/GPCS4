/*
 *    GPCS4
 *    
 *    This file implements:
 *    module: libScePngDec
 *        library: libScePngDec
 *    
 */

#pragma once

#include "sce_module_common.h"
#include "sce_pngdec_types.h"

extern const SCE_EXPORT_MODULE g_ExpModuleScePngDec;


// Note:
// The codebase is generated using GenerateCode.py
// You may need to modify the code manually to fit development needs



//////////////////////////////////////////////////////////////////////////
// library: libScePngDec
//////////////////////////////////////////////////////////////////////////


int PS4API scePngDecDecode(ScePngDecHandle handle, const ScePngDecDecodeParam* param, ScePngDecImageInfo* imageInfo);


