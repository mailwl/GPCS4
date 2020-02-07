#include "sce_pngdec.h"


// Note:
// The codebase is generated using GenerateCode.py
// You may need to modify the code manually to fit development needs


static const SCE_EXPORT_FUNCTION g_pScePngDec_libScePngDec_FunctionTable[] =
{
	{ 0x582DB5E830F7125E, "scePngDecDecode", (void*)scePngDecDecode },
	SCE_FUNCTION_ENTRY_END
};

static const SCE_EXPORT_LIBRARY g_pScePngDec_LibTable[] =
{
	{ "libScePngDec", g_pScePngDec_libScePngDec_FunctionTable },
	SCE_LIBRARY_ENTRY_END
};

const SCE_EXPORT_MODULE g_ExpModuleScePngDec =
{
	"libScePngDec",
	g_pScePngDec_LibTable
};


