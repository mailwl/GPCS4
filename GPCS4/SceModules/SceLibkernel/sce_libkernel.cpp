#include "sce_libkernel.h"

#include "pthreads4w/pthread.h"

#include "Platform/PlatformUtils.h"

#ifdef GPCS4_WINDOWS
#include <Windows.h>
#endif

// Note:
// The codebase is generated using GenerateCode.py
// You may need to modify the code manually to fit development needs

//////////////////////////////////////////////////////////////////////////
// library: libkernel
//////////////////////////////////////////////////////////////////////////

int* PS4API __error(void)
{
	LOG_FIXME("Not implemented");
	return &errno;
}

int PS4API __stack_chk_fail(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API __stack_chk_guard(void)
{
	LOG_FIXME("Not implemented");
	return 1;
}

void* PS4API __tls_get_addr(const uint64_t* slot)
{
	LOG_DEBUG("slot: %d", *slot);
	static char fake[0x10000];
	//return nullptr;  // TODO: implement this
	return fake;
}

int PS4API __pthread_cxa_finalize(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelClockGettime(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelGetCpumode(void)
{
	LOG_SCE_DUMMY_IMPL();
	return SCE_KERNEL_CPUMODE_7CPU_NORMAL;
}

// Is PS4 Pro
int PS4API sceKernelIsNeoMode(void)
{
	int isNeoMode = 1;
	LOG_SCE_TRACE("return %d", isNeoMode);
	return isNeoMode;
}

int PS4API sceKernelUsleep(SceKernelUseconds microseconds)
{
	//LOG_SCE_TRACE("ms %d", microseconds);
	UtilTime::MicroSleep(microseconds);
	return SCE_OK;
}

int PS4API sceKernelBatchMap(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelCheckedReleaseDirectMemory(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelDlsym(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelGetGPI(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelGettimeofday(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelLoadStartModule(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelSetPrtAperture(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelUuidCreate(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

void PS4NORETURN PS4API sceKernelDebugRaiseException(uint32_t error_code, uint32_t param)
{
	LOG_FIXME("Not implemented");
	exit(-1);
}

void PS4API sceKernelDebugRaiseExceptionOnReleaseMode(uint32_t error_code, uint32_t param)
{
	LOG_FIXME("Not implemented");
}

int PS4API sceCoredumpWriteUserData(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelGetCompiledSdkVersion(uint64_t* version)
{
	LOG_SCE_TRACE("");
	*version = 4500;
	return SCE_OK;
}

int PS4API sceKernelIsAddressSanitizerEnabled()
{
	LOG_SCE_TRACE("");
	return false;
}

struct proc7
{
	uint64_t size{ 0x40 };  // 00
	uint64_t p1{};          // 01
	uint64_t p2{};          // 02
	uint64_t p3{};          // 03
	uint64_t p4{};          // 04
	uint64_t p5{};          // 05
	uint64_t p6{};          // 06
	uint64_t p7{};          // 07
	uint64_t p8{};          // 08
	uint64_t p9{};          // 09
	uint64_t p10{};         // 10
	uint64_t p11{};         // 11
	uint64_t* p12{nullptr}; // 12
} p7;

struct ProcParam
{
	uint64_t size{ 0x40 };  // 00
	uint64_t p1{};          // 01
	uint64_t p2{};          // 02
	uint64_t p3{};          // 03
	uint64_t p4{};          // 04
	uint64_t p5{};          // 05
	uint64_t p6{};          // 06
	proc7* p7{};            // 07
} procParam;
void* PS4API sceKernelGetProcParam(uint64_t p1, uint64_t p2)
{
	LOG_SCE_DUMMY_IMPL();
	procParam.p7 = &p7;
	return &procParam;
}

void PS4API _sceKernelRtldSetApplicationHeapAPI(void* heap_api)
{
	LOG_SCE_DUMMY_IMPL();
}

int PS4API scePthreadAttrGet(ScePthread thread, ScePthreadAttr* attr)
{
	LOG_SCE_DUMMY_IMPL();
	*attr = nullptr;
	return SCE_OK;
}

int PS4API scePthreadAttrGetaffinity(ScePthread thread, SceKernelCpumask* mask)
{
	LOG_SCE_DUMMY_IMPL();
	*mask = 0;
	return SCE_OK;
}

//////////////////////////////////////////////////////////////////////////
// library: libSceCoredump
//////////////////////////////////////////////////////////////////////////

int PS4API sceCoredumpAttachMemoryRegion(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceCoredumpRegisterCoredumpHandler(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API sceKernelGetModuleInfoForUnwind(void* out, int kind, int* out_size)
{
	LOG_FIXME("Not implemented");
	int rc = -1;  // TODO: implement this
	return rc;
}

//////////////////////////////////////////////////////////////////////////
// library: libScePosix
//////////////////////////////////////////////////////////////////////////

int PS4API scek_sched_yield(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_close(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_connect(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_recv(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_select(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_sem_destroy(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_sem_init(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_sem_post(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_sem_timedwait(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_sem_wait(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_send(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_shutdown(void)
{
	LOG_FIXME("Not implemented");
	return SCE_OK;
}

int PS4API scek_getpid(void)
{
	int pid = 0x1337;
	LOG_SCE_TRACE("return %d", pid);
	return pid;
}

int PS4API scek_getppid(void)
{
	int pid = 0x1;
	LOG_SCE_TRACE("return %d", pid);
	return pid;
}


int PS4API scek_sigfillset(sce_sigset_t* set)
{
	LOG_SCE_DUMMY_IMPL();
	return SCE_OK;
}


int PS4API scek_sigprocmask(int how, const sce_sigset_t* set, sce_sigset_t* old_set)
{
	LOG_SCE_DUMMY_IMPL();
	return SCE_OK;
}


int PS4API scek_getpagesize()
{
	LOG_SCE_DUMMY_IMPL();
	return 4096;
}


void* PS4API scek_mmap(void* start, size_t length, int prot, int flags, int fd, off_t offset)
{
	LOG_SCE_TRACE("%p, %zu, %x, %x, %d, %d", start, length, prot, flags, fd, offset);
#ifdef GPCS4_WINDOWS
	auto const mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE | SEC_COMMIT, 0, length, nullptr);
	auto const view    = MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, length);
	return view;
#else
	// TODO: convert flags from PS4 to Linux
	return mmap(start, length, prot, flags, fd, offset);
#endif
}


int PS4API _is_signal_return(uint64_t param)
{
	LOG_SCE_DUMMY_IMPL();
	return SCE_OK;
}


int PS4API scek_msync(void* start, size_t length, int flags)
{
#ifdef GPCS4_WINDOWS
	return 0;
#else
	return msync(start, length, flags);
#endif
}
