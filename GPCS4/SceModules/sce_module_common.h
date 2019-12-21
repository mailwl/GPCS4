#pragma once

#include "GPCS4Common.h"
#include "sce_errors.h"
#include "sce_types.h"

struct SCE_EXPORT_FUNCTION
{
	const uint64 nNid;
	const char* szFunctionName;
	const void* pFunction;
};
#define SCE_FUNCTION_ENTRY_END \
	{                          \
		0, NULL, NULL          \
	}

struct SCE_EXPORT_LIBRARY
{
	const char* szLibraryName;
	const SCE_EXPORT_FUNCTION* pFunctionEntries;
};
#define SCE_LIBRARY_ENTRY_END \
	{                         \
		NULL, NULL            \
	}

struct SCE_PRX_NAME
{
	const char* szPrxName;
};

struct SCE_EXPORT_MODULE
{
	const char* szModuleName;
	const SCE_EXPORT_LIBRARY* pLibraries;
	const SCE_PRX_NAME* pKnownPrxNames;
	//bool isKnownPrxName(const char* szPrxName)
	//{
	//	bool bRet = false;
	//	do
	//	{
	//		if (pKnownPrxNames == nullptr)
	//		{
	//			break;
	//		}
	//		int i = 0;
	//		while (pKnownPrxNames[i])
	//		{
	//			if (!strcmp(szPrxName, pKnownPrxNames[i])) {
	//				bRet = true;
	//				break;
	//			}
	//			++i;
	//		}
	//	} while (false);
	//	return bRet;
	//}
};
