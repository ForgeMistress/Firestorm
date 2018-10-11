#include "stdafx.h"
#include "libCore.h"
#include "Logger.h"
#include <sstream>

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

void libCore::SetThreadName(Thread& thread, const String& name)
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
	String file;
	size_t line;
};
static UnorderedMap<void*, AllocInfo> _s_liveAllocations;
#endif

#ifndef FIRE_FINAL
void* libCore::Alloc(size_t sizeInBytes, const char* file, size_t line)
{
	void* ptr = malloc(sizeInBytes);
	_s_liveAllocations[ptr] = {
		sizeInBytes,
		file,
		line
	};
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
			FIRE_LOG_ERROR("Leak => 0x%f%f%f%a -> %d bytes (file: %s, line: %d)",
				std::hex, std::setw(sizeof(allocation.first)*2), std::setfill('0'),
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
Vector<String> SplitString(const String & str, char delim)
{
	Vector<String> tokens;
	String token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}
CLOSE_NAMESPACE(Firestorm);
