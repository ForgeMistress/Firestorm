#include "stdafx.h"
#include "libCore.h"
#include "Logger.h"
#include <sstream>
#include <EASTL/string.h>

#define _printt(type) \
	std::cout << "    :: "#type" -> Max = " << std::numeric_limits<type>::max() <<" Size = "<<sizeof(type)<< std::endl

OPEN_NAMESPACE(Firestorm);

void libCore::Initialize(int ac, char** av)
{
	_printt(unsigned);
	_printt(uint32_t);
	_printt(uint64_t);
}
CLOSE_NAMESPACE(Firestorm);

#ifdef FIRE_PLATFORM_WINDOWS
#include <windows.h>

OPEN_NAMESPACE(Firestorm);
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void libCore::SetThreadName(Thread& thread, const string& name)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name.c_str();
	info.dwThreadID = GetThreadId(static_cast<HANDLE>(thread.native_handle()));
	info.dwFlags = 0;
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

#ifndef FIRE_FINAL
struct AllocInfo
{
	size_t allocSize;
	const char* file;
	size_t line;
};
static unordered_map<void*, AllocInfo> _s_liveAllocations;
#endif

#ifndef FIRE_FINAL
void* libCore::Alloc(size_t sizeInBytes, const char* file, size_t line)
{
	void* ptr = malloc(sizeInBytes);
	//_s_liveAllocations[ptr] = {
	//	sizeInBytes,
	//	file?file:"",
	//	line
	//};
	return ptr;
#else
void* libCore::Alloc(size_t sizeInBytes)
{
	return malloc(sizeInBytes);
#endif
}

void libCore::Free(void* block)
{
#ifndef FIRE_FINAL
	_s_liveAllocations.erase(block);
#endif
	free(block);
}

void libCore::ReportMemoryLeaks()
{
#ifndef FIRE_FINAL
	FIRE_LOG_DEBUG("");
	FIRE_LOG_DEBUG("======= LEAK CHECK =======");
	if(_s_liveAllocations.empty())
	{
		FIRE_LOG_DEBUG("No memory leaks. It's all good fam.");
	}
	else
	{
		for(auto& allocation : _s_liveAllocations)
		{
			AllocInfo& info = allocation.second;
			FIRE_LOG_ERROR("Leak => %#x -> %d bytes (file: %s, line: %d)",
				allocation.first, info.allocSize, info.file, info.line);
		}
	}
	FIRE_LOG_DEBUG("===== END LEAK CHECK =====");
	FIRE_LOG_DEBUG("");
#endif
}

CLOSE_NAMESPACE(Firestorm);

#endif

OPEN_NAMESPACE(Firestorm);
vector<string> SplitString(const string & str, char delim)
{
	return vector<string>();
}
CLOSE_NAMESPACE(Firestorm);


void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::Firestorm::libCore::Alloc(size, file, line);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::Firestorm::libCore::Alloc(size, file, line);
}
