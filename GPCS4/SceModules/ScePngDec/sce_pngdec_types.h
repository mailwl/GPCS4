#pragma once

#include <stdint.h>


typedef void* ScePngDecHandle;


typedef struct ScePngDecDecodeParam
{
    const void* pngMemAddr;
    void* imageMemAddr;
    uint32_t pngMemSize;
    uint32_t imageMemSize;
    uint16_t pixelFormat;
    uint16_t alphaValue;
    uint32_t imagePitch;
} ScePngDecDecodeParam;


typedef struct ScePngDecImageInfo
{
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint16_t colorSpace;
    uint16_t bitDepth;
    uint32_t imageFlag;
} ScePngDecImageInfo;



