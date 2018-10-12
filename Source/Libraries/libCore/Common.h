
#ifndef LIBCORE_COMMON_H_
#define LIBCORE_COMMON_H_
#pragma once

#include <EASTL/vector.h>
#include <EASTL/memory.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>
#include <EASTL/string.h>
#include <EASTL/list.h>
#include <EASTL/queue.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/functional.h>
#include <EASTL/type_traits.h>
#include <EASTL/chrono.h>
#include <EASTL/tuple.h>
#include <cassert>
#include <stdint.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <future>
#include <sstream>
#include <ostream>
#include <new>
#include <iomanip>
#include <iostream>
#include <typeinfo>
#include <typeindex>

namespace Firestorm
{

using namespace eastl;

template<class T>
using future = std::future<T>;

template<class T>
using promise = std::promise<T>;

static const string str_Default;

using uchar = unsigned char;

template<class T> using Atomic = std::atomic<T>;

using Thread = std::thread;

using Mutex = std::mutex;

using ID = uint64_t;


}

#endif