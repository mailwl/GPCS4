#include "sce_pngdec.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

// Note:
// The codebase is generated using GenerateCode.py
// You may need to modify the code manually to fit development needs

LOG_CHANNEL(SceModules.ScePngDec);

//////////////////////////////////////////////////////////////////////////
// library: libScePngDec
//////////////////////////////////////////////////////////////////////////


int PS4API scePngDecDecode(ScePngDecHandle handle, const ScePngDecDecodeParam* param, ScePngDecImageInfo* imageInfo) 
{
    LOG_DEBUG("(%p, %p, %p)", handle, param, imageInfo);

    int w, h, n;
    auto image = stbi_load_from_memory((const stbi_uc*) param->pngMemAddr, param->pngMemSize, &w, &h, &n, 4);
    std::memcpy(param->imageMemAddr, image, param->imageMemSize);
    free(image);
    return  ((uint16_t)w << 16u) | (uint16_t)h;
}
