#ifndef LIBCORE_H_
#define LIBCORE_H_
#pragma once

#ifdef LIB_CORE
#error "libCore.h already included"
#endif

#define LIB_CORE

#define OPEN_NAMESPACE(ns) namespace ns { extern int ns_##ns
#define CLOSE_NAMESPACE(ns) } // namespace ns

#include <vector>
#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cassert>
#include <functional>
#include <stdint.h>
#include <type_traits>
#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include "Expected.h"
#include "LibraryRegistrar.h"

OPEN_NAMESPACE(Firestorm);

template<
	typename Expected_t, 
	typename Unexpected_t
> using Expected = tl::expected<Expected_t, Unexpected_t>;

using std::cout;
using std::endl;

using std::function;
template <class _Fty> using Function = std::function<_Fty>;

using std::pair;
template <class L, class R> using Pair = std::pair<L, R>;

template<
	class Ty,
	class Alloc = std::allocator<Ty>
> using Vector = std::vector<Ty, Alloc>;

using DataBuffer = Vector<char>;

using String = std::string;

template <
	class Ty,
	class Alloc = std::allocator<Ty>
> using List = std::list<Ty, Alloc>;

template <
	class Kty,
	class Ty,
	class Hasher = std::hash<Kty>,
	class Keyeq = std::equal_to<Kty>,
	class Alloc = std::allocator<std::pair<const Kty, Ty>>
> using UnorderedMap = std::unordered_map<Kty, Ty, Hasher, Keyeq, Alloc>;

template<class Kty,
	class Hasher = std::hash<Kty>,
	class Keyeq = std::equal_to<Kty>,
	class Alloc = std::allocator<Kty>
> using UnorderedSet = std::unordered_set<Kty, Hasher, Keyeq, Alloc>;

static const String str_Default;

using uchar = unsigned char;

template<class T> using Atomic = std::atomic<T>;

using Thread = std::thread;

using Mutex = std::mutex;

using SharedMutex = std::shared_mutex;

// Aliases for type_trait structs in type_traits (C++11)
OPEN_NAMESPACE(Traits);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primary Type Categories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsVoid = std::is_void<T>;
template <class T> using IsNullPointer = std::is_null_pointer<T>;
template <class T> using IsIntegral = std::is_integral<T>;
template <class T> using IsFloatingPoint = std::is_floating_point<T>;
template <class T> using IsArray = std::is_array<T>;
template <class T> using IsEnum = std::is_enum<T>;
template <class T> using IsUnion = std::is_union<T>;
template <class T> using IsPointer = std::is_pointer<T>;
template <class T> using IsClass = std::is_class<T>;
template <class T> using IsLValueReference = std::is_lvalue_reference<T>;
template <class T> using IsRValueReference = std::is_rvalue_reference<T>;
template <class T> using IsMemberObjectPointer = std::is_member_object_pointer<T>;
template <class T> using IsMemberFunctionPointer = std::is_member_function_pointer<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Composite Type Categories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsFundamental = std::is_fundamental<T>;
template <class T> using IsArithmetic = std::is_arithmetic<T>;
template <class T> using IsScalar = std::is_scalar<T>;
template <class T> using IsObject = std::is_object<T>;
template <class T> using IsCompound = std::is_compound<T>;
template <class T> using IsReference = std::is_reference<T>;
template <class T> using IsMemberPointer = std::is_member_pointer<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type Properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using IsConst = std::is_const<T>;
template <class T> using IsVolatile = std::is_volatile<T>;
template <class T> using IsTrivial = std::is_trivial<T>;
template <class T> using IsTriviallyCopyable = std::is_trivially_copyable<T>;
template <class T> using IsStandardLayout = std::is_standard_layout<T>;
template <class T> using IsPOD = std::is_pod<T>; // C++20
// template <class T> using IsLiteralType                  = std::is_literal_type<T>; // Deprecated by C++17
// template <class T> using HasUniqueObjectRepresentations = std::has_unique_object_representations<T>; // C++17
template <class T> using IsEmpty = std::is_empty<T>;
template <class T> using IsPolymorphic = std::is_polymorphic<T>;
template <class T> using IsAbstract = std::is_abstract<T>;
template <class T> using IsFinal = std::is_final<T>;
// template <class T> using IsAggregate = std::is_aggregate<T>;
template <class T> using IsSigned = std::is_signed<T>;
template <class T> using IsUnsigned = std::is_unsigned<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Const-Volatility Specifiers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemoveCV = std::remove_cv<T>;
template <class T> using RemoveConst = std::remove_const<T>;
template <class T> using RemoveVolatile = std::remove_volatile<T>;
template <class T> using AddCV = std::add_cv<T>;
template <class T> using AddConst = std::add_const<T>;
template <class T> using AddVolatile = std::add_volatile<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// References
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemoveReference = std::remove_reference<T>;
template <class T> using AddLValueReference = std::add_lvalue_reference<T>;
template <class T> using AddRValueReference = std::add_rvalue_reference<T>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pointers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> using RemovePointer = std::remove_pointer<T>;
template <class T> using AddPointer = std::add_pointer<T>;

CLOSE_NAMESPACE(Traits);

struct libCore : public Library<libCore>
{
	FIRE_LIBRARY(libCore);
private:
	static void Initialize(int ac, char** av);
};

extern Vector<String> SplitString(const String& str, char delim);

template<class T>
extern void Remove(Vector<T>& v, const T& value)
{
	v.erase(std::remove(v.begin(), v.end(), value), v.end());
}

template <class T, class U>
extern bool Exists(const T& search, const U& value)
{
	if(std::find(search.begin(), search.end(), value) == search.end())
		return false;
	return true;
}

template <class T>
extern T* PlacementNew(void* place)
{
	return new (place) T;
}

CLOSE_NAMESPACE(Firestorm);

/**
	Use this to flag a variable as unused in a function.
 **/
#define FIRE_UNUSED_VARIABLE(var) ((void)var)

#define FIRE_ASSERT assert

#define FIRE_BROKEN FIRE_ASSERT(false && "it's fucking broken. don't use this.")

#endif