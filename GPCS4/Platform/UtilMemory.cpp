#include "UtilMemory.h"

namespace UtilMemory
{;

#define ALIGN_MEMORY 0x1000000

#ifdef GPCS4_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

inline uint GetProtectFlag(uint nOldFlag)
{
	uint nNewFlag = 0;
	do 
	{
		if (nOldFlag & VMPF_NOACCESS)
		{
			nNewFlag |= PAGE_NOACCESS;
			break;
		}

		if (nOldFlag & VMPF_READ)
		{
			nNewFlag = PAGE_READONLY;
		}

		if (nOldFlag & VMPF_WRITE)
		{
			nNewFlag = PAGE_READWRITE;
		}

		if (nOldFlag & VMPF_EXECUTE)
		{
			nNewFlag = PAGE_EXECUTE_READWRITE;
		}

	} while (false);
	return nNewFlag;
}

inline uint GetTypeFlag(uint nOldFlag)
{
	uint nNewFlag = 0;
	do
	{
		if (nOldFlag & VMAT_RESERVE)
		{
			nNewFlag |= MEM_RESERVE;
		}

		if (nOldFlag & VMAT_COMMIT)
		{
			nNewFlag |= MEM_COMMIT;
		}

	} while (false);
	return nNewFlag;
}

void* VMMap(size_t nSize, uint nProtectFlag)
{
	void* pAddr = NULL;
	do 
	{
		//DWORD dummy;
		//pAddr = _aligned_malloc(nSize, ALIGN_MEMORY);
		//bool rc = VirtualProtect(pAddr, nSize, GetProtectFlag(nProtectFlag), &dummy);
		//if (!rc)
		//{
		//	LOG_ERR("Can not change memory protection!");
		//}
		pAddr = VirtualAlloc(NULL, nSize, MEM_RESERVE | MEM_COMMIT, GetProtectFlag(nProtectFlag));
	} while (false);
	return pAddr;
}

void* VMMapEx(void* pAddr, size_t nSize, uint nProtectFlag, uint nType)
{
	do
	{
		//DWORD dummy;
		//pAddr = _aligned_malloc(nSize, ALIGN_MEMORY);
		//bool rc = VirtualProtect(pAddr, nSize, GetProtectFlag(nProtectFlag), &dummy);
		//if (!rc)
		//{
		//	LOG_ERR("Can not change memory protection!");
		//}
		pAddr = VirtualAlloc(pAddr, nSize, GetTypeFlag(nType), GetProtectFlag(nProtectFlag));
	} while (false);
	return pAddr;
}

void VMUnMap(void* pAddr, size_t nSize)
{
	VirtualFree(pAddr, nSize, MEM_RELEASE);
	//_aligned_free(pAddr);
}

bool VMProtect(void* pAddr, size_t nSize, uint nProtectFlag)
{
	LOG_FIXME("Not Implemented.");
	return  true;
}

#elif defined(GPCS4_LINUX)

//TODO: Other platform implementation 

#endif  //GPCS4_WINDOWS
}