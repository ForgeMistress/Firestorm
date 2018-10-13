#include "stdafx.h"
#include "libCore.h"
#include "Logger.h"
#include <sstream>
#include <EASTL/string.h>
#include "Assert.h"

void* FIRE_ALLOC(size_t size)
{
	return malloc(size);
}

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
#include <malloc.h>
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
#endif
OPEN_NAMESPACE(Firestorm);

#ifdef FIRE_PLATFORM_WINDOWS
void libCore::SetThreadName(thread& thread, const string& name)
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
#endif

void* libCore::Alloc(size_t sizeInBytes)
{
	return malloc(sizeInBytes);
}

void* libCore::AlignedAlloc(size_t sizeInBytes, size_t alignment)
{
	return malloc(sizeInBytes);
}

void libCore::Free(void* block)
{
	free(block);
}

void libCore::ReportMemoryLeaks()
{
}

vector<string> SplitString(const string & str, char delim)
{
	vector<string> cont;
	std::size_t current, previous = 0;
	current = str.find(delim);
	while (current != std::string::npos) {
		cont.push_back(str.substr(previous, current - previous));
		previous = current + 1;
		current = str.find(delim, previous);
	}
	cont.push_back(str.substr(previous, current - previous));
	return cont;
	//string s(str);
	//vector<string> out;
	//size_t pos = 0;
	//while((pos = s.find(delim)) != string::npos)
	//{
	//	out.push_back(s.substr(0, pos));
	//	s.erase(0, pos + 1);
	//}
	//return out;
}
CLOSE_NAMESPACE(Firestorm);

// support for EASTL
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::Firestorm::libCore::AlignedAlloc(size, 16);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return ::Firestorm::libCore::AlignedAlloc(size, alignment);
}

void operator delete(void* p) EA_THROW_SPEC_DELETE_NONE()
{
	::Firestorm::libCore::Free(p);
}

void operator delete[](void* p) EA_THROW_SPEC_DELETE_NONE()
{
	::Firestorm::libCore::Free(p);
}