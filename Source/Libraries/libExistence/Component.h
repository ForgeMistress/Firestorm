///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Component.h
//
//  A component in an entity that contains only data at the high level.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_COMPONENT_H_
#define LIBEXISTENCE_COMPONENT_H_
#pragma once

#include <libCore/libCore.h>
#include "Entity.h"
#include <libCore/Logger.h>
#include <utility>
#include <variant>
OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(detail);

template<class T>
struct add_ptr
{
	using type = T*;
};

// https://stackoverflow.com/questions/1198260/iterate-over-tuple

template<
	size_t Index = 0, // start iteration at 0 index
	typename TTuple,  // the tuple type
	size_t Size =
		std::tuple_size_v<
			std::remove_reference_t<TTuple>>, // tuple size
	typename TCallable, // the callable to bo invoked for each tuple item
	typename... TArgs   // other arguments to be passed to the callable 
>
void for_each(TTuple&& tuple, TCallable&& callable, TArgs&&... args)
{
	if constexpr (Index < Size)
	{
		std::invoke(callable, args..., std::get<Index>(tuple));

		if constexpr (Index + 1 < Size)
			for_each<Index + 1>(
				std::forward<TTuple>(tuple),
				std::forward<TCallable>(callable),
				std::forward<TArgs>(args)...);
	}
}

/*template<typename Tuple>
typename std::tuple_element<0, typename std::remove_reference<Tuple>::type&
	runtime_get(Tuple&& t, size_t index)
{

}*/

// https://www.justsoftwaresolutions.co.uk/cplusplus/getting-tuple-elements-with-runtime-index.html

/*template<
	typename TTuple,
	typename Indices=std::make_index_sequence<std::tuple_size<TTuple>::value>>
	struct runtime_get_func_table;

template<typename TTuple,size_t ... Indices>
struct runtime_get_func_table<TTuple,std::index_sequence<Indices...>>{
	using return_type=typename std::tuple_element<0,TTuple>::type&;
	using get_func_ptr=return_type (*)(TTuple&) noexcept;
	static constexpr get_func_ptr table[std::tuple_size<TTuple>::value]={
		&std::get<Indices>...
	};
};

template<typename TTuple,size_t ... Indices>
constexpr typename
runtime_get_func_table<TTuple,std::index_sequence<Indices...>>::get_func_ptr
runtime_get_func_table<TTuple,std::index_sequence<Indices...>>::table[std::tuple_size<TTuple>::value];

template<typename TTuple>
constexpr
typename std::tuple_element<0,typename std::remove_reference<TTuple>::type>::type&
runtime_get(TTuple& t,size_t index){
	using tuple_type=typename std::remove_reference<TTuple>::type;
	if(index>=std::tuple_size<tuple_type>::value)
		throw std::runtime_error("Out of range");
	return runtime_get_func_table<tuple_type>::table[index](t);
}*/

template<std::size_t I>
struct tuple_index {};

template<char... Digits>
constexpr std::size_t parse()
{
	char digits[] = { Digits... };
	auto result = 0u;
	for(auto c : digits)
	{
		result *= 10;
		result += c - '0';
	}
	return result;
}






template <typename Tup, typename R, typename F, std::size_t... Idxs>
struct tuple_runtime_access_table {
	using tuple_type = Tup;
	using return_type = R;
	using converter_fun = F;

	template <std::size_t N>
	static return_type access_tuple(tuple_type& t, converter_fun& f) {
		return f(std::get<N>(t));
	}

	using accessor_fun_ptr = return_type(*)(tuple_type&, converter_fun&);
	const static auto table_size = sizeof...(Idxs);

	constexpr static std::array<accessor_fun_ptr, table_size> lookup_table = {
		{&access_tuple<Idxs>...}
	};
};


template <typename R, typename Tup, typename F, std::size_t... Idxs>
auto call_access_function(Tup& t, std::size_t i, F f, std::index_sequence<Idxs...>) {
	auto& table = tuple_runtime_access_table<Tup, R, F, Idxs...>::lookup_table;
	auto* access_function = table[i];
	return access_function(t, f);
}



template <typename Tup> struct common_tuple_access;

template <typename... Ts>
struct common_tuple_access<std::tuple<Ts...>> {
	using type = std::variant<std::reference_wrapper<Ts>...>;
};

template <typename T1, typename T2>
struct common_tuple_access<std::pair<T1, T2>> {
	using type = std::variant<std::reference_wrapper<T1>, std::reference_wrapper<T2>>;
};

template <typename T, auto N>
struct common_tuple_access<std::array<T, N>> {
	using type = std::variant<std::reference_wrapper<T>>;
};

template <typename Tup>
using common_tuple_access_t = typename common_tuple_access<Tup>::type;



template <typename Tup>
auto runtime_get(Tup& t, std::size_t i) {
	return call_access_function<common_tuple_access_t<Tup>>(
		t, i, 
		[](auto & element){ return std::ref(element); },
		std::make_index_sequence<std::tuple_size_v<Tup>>{}
	);
}



template <typename Tup> class tuple_iterator {
	Tup& t;
	size_t i;
public:
	tuple_iterator(Tup& tup, size_t idx)
		: t{tup}, i{idx} 
	{}

	tuple_iterator& operator++() { 
		++i; return *this; 
	}
	bool operator==(tuple_iterator const& other) const {
		return std::addressof(other.t) == std::addressof(t)
			&& other.i == i;
	}

	bool operator!=(tuple_iterator const& other) const {
		return !(*this == other);
	}

	auto operator*() const{ 
		return runtime_get(t, i); 
	}
};

template <typename Tup>
class to_range {
	Tup& t;
public:    
	to_range(Tup& tup) : t{tup}{}
	auto begin() {
		return tuple_iterator{t, 0};
	}
	auto end() {
		return tuple_iterator{t, std::tuple_size_v<Tup>};
	}

	auto operator[](std::size_t i){
		return runtime_get(t, i);
	}
};

template<class T>
using requires_destructor_call_v = typename std::enable_if<
	std::is_class<T>::value &&
	!std::is_arithmetic<T>::value &&
	std::is_destructible<T>::value &&
	(
		!std::is_trivially_destructible<T>::value ||
		std::has_virtual_destructor<T>::value
	)>::value;

template<class T>
using requires_destructor_call = typename std::enable_if<
	(
		std::is_class<T>::value &&
		!std::is_arithmetic<T>::value &&
		std::is_destructible<T>::value &&
		(
			!std::is_trivially_destructible<T>::value ||
			std::has_virtual_destructor<T>::value
		)
	) || std::is_same_v<String, T>>;

template<class T>
using requires_default_constructor_call = typename std::enable_if<
	std::is_class<T>::value &&
	!std::is_arithmetic<T>::value &&
	std::is_constructible<T>::value &&
	(
		std::is_default_constructible<T>::value
	)>::value;

template<class T>
using does_not_require_destructor_call = typename std::enable_if<
	std::is_arithmetic<T>::value
	>::value;

CLOSE_NAMESPACE(detail);

template<class T> struct MemberData;

struct MemberBufferTraits
{
	struct constructible_tag{};
	struct call_destructor{};

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>
	>
	static inline void Alloc(Mem** mem, size_t numElements)
	{
		*mem = (Mem*)malloc(numElements * sizeof(Mem));
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename detail::requires_destructor_call_v<Mem>* = nullptr
	> static inline void FreeItem(Mem* buffer, size_t item)
	{
		Mem* item = &buffer[i];
		if(item)
		{
			item->~Mem();
			memset(item, 0, sizeof(Mem));
		}
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename detail::requires_destructor_call_v<Mem>* = nullptr
	>
	static inline void Free(Mem* buffer, size_t numElements)
	{
		for(size_t i = 0; i < numElements; ++i)
		{
			Mem* item = &buffer[i];
			if(item)
			{
				item->~Mem();
				memset(item, 0, sizeof(Mem));
			}
		}
		free(buffer);
	}

	static inline void Free(String* buffer, size_t numElements)
	{
		for(size_t i = 0; i < numElements; ++i)
		{
			String* item = &buffer[i];
			if(item)
			{
				item->~String();
				memset(item, 0, sizeof(String));
			}
		}
		free(buffer);
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename detail::does_not_require_destructor_call<Mem>* = nullptr
	> static inline void FreeItem(Mem* buffer, size_t item)
	{
		memset(&buffer[item], 0, sizeof(Mem));
	}

	static inline void FreeItem(String* buffer, size_t item)
	{
		memset(&buffer[item], 0, sizeof(String));
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>
	>
	static inline void Free(Mem* buffer, size_t numElements)
	{
		free(buffer);
	}

	template<
		class MemberT,
		class... ArgsT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename = typename std::enable_if<std::is_constructible<MemberT>::value,constructible_tag>::type
	>
	static inline Mem* New(Mem* buffer, size_t currentSize, ArgsT... args)
	{
		Mem* item = &buffer[currentSize];
		FIRE_ASSERT(!item);
		new (item) Mem(std::forward<ArgsT>(args)...);
		return item;
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>
	> static inline Mem* New(Mem* buffer, size_t currentSize)
	{
		Mem* item = &buffer[currentSize];
		memset(item, 0, sizeof(Mem));
		return item;
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>
	> static inline Mem* Get(Mem* buffer, size_t index)
	{
		return &buffer[index];
	}

	struct scalar_tag {};
	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename tag = typename std::enable_if<std::is_scalar<Mem>::value,scalar_tag>::type
	> static inline void Copy(Mem* buffer, size_t index, const Mem* value, scalar_tag* t = nullptr)
	{
		Mem* inst = &buffer[index];
		memcpy(inst, value, sizeof(Mem));
	}

	struct copyable_class_tag{};
	/*template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename tag = typename std::enable_if<
			std::is_class<Mem>::value &&
			std::is_copy_constructible<Mem>::value, copyable_class_tag>::type
	> static inline void Copy(Mem* buffer, size_t index, const Mem* value, copyable_class_tag* t=nullptr)
	{
		Mem* inst = &buffer[index];
		if(inst)
		{
			if constexpr(std::is_copy_assignable_v<Mem>)
			{
				*inst = *value;
			}
		}
		else
		{
			new(inst) Mem(*value);
		}
	}*/

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename tag = typename std::enable_if<
			std::is_class<Mem>::value, copyable_class_tag>::type
	> static inline void Copy(Mem* buffer, size_t index, const Mem* value)
	{
		Mem* inst = &buffer[index];
		FIRE_ASSERT(inst);
		if(inst)
		{
			if constexpr(std::is_copy_assignable_v<Mem>)
			{
				*inst = *value;
			}
		}
		else
		{
			new(inst) Mem(*value);
		}
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename tag = typename std::enable_if<
			std::is_scalar<Mem>::value
		>::type
	> static inline void Move(Mem* buffer, size_t index, Mem&& value)
	{
		Mem* inst = &buffer[index];
		memmove(inst, &value, sizeof(Mem));
	}

	template<
		class MemberT,
		typename Mem = std::remove_pointer_t<MemberT>,
		typename tag = typename std::enable_if<
			std::is_class<Mem>::value &&
			std::is_move_constructible<Mem>::value, copyable_class_tag>::type
	> static inline void Move(Mem* buffer, size_t index, Mem&& value, copyable_class_tag* t=nullptr)
	{
		Mem* inst = &buffer[index];
		new(inst) Mem(std::move(value));
	}
};


template<class... MembersT>
struct ComponentDefinition : public std::tuple<std::add_pointer_t<MembersT>...>
{
	using tuple_type = std::tuple<std::add_pointer_t<MembersT>...>;
	static constexpr std::size_t LLength = sizeof...(MembersT);
	using def_type = ComponentDefinition<MembersT...>;
	size_t Size;
	size_t Capacity;

	ComponentDefinition()
	: std::tuple<std::add_pointer_t<MembersT>...>()
	, Size(0)
	, Capacity(0)
	{
	}

	virtual ~ComponentDefinition()
	{
		detail::for_each((tuple_type&)(*this), [this](auto& buffer) {
			MemberBufferTraits::Free<decltype(buffer)>(buffer, Size);
		});
	}

	void Alloc(size_t numMembers)
	{
		size_t i = 0;
		detail::for_each((tuple_type&)(*this), [numMembers](auto& buffer) {
			MemberBufferTraits::Free<decltype(buffer)>(buffer, numMembers);
			MemberBufferTraits::Alloc<decltype(buffer)>(&buffer, numMembers);
		});
		Capacity = numMembers;
		Size = 0;
	}

	/*template<class... Args>
	void MakeInstance(Args... args)
	{
		for(size_t i=0;i<LLength;++i)
		{
			auto& val = detail::runtime_get((tuple_type&)*this, i);
			MemberBufferTraits::NewInstance<std::remove_reference_t<decltype(val)>>(val, Size, std::forward<Args>(args)...);
			//auto val = detail::runtime_get<tuple_type>((tuple_type&)*this, i);
			//auto val = (*this)[i_i];
			//MemberBufferTraits::NewInstance<decltype(val)>(val, Size, std::forward<Args>(args)...);
		}
		++Size;
	}

	void MakeInstance()
	{
		for(size_t i=0;i<LLength;++i)
		{
			auto& val = detail::runtime_get((tuple_type&)*this, i);
			MemberBufferTraits::NewInstance<std::remove_reference_t<decltype(val)>>(val, Size);
			//auto val = detail::runtime_get<tuple_type>((tuple_type&)*this, i);
			//auto val = (*this)[i_i];
			//MemberBufferTraits::NewInstance<decltype(val)>(val, Size);
		}
		++Size;
	}*/

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	void New()
	{
		static_assert(
			std::is_same<T, ElemT>::value,
			"Invalid type passed to New. The field types must be the _exact_ same.");
		MemberBufferTraits::New<T>(std::get<MemberIndex>((tuple_type&)*this), Size);
	}

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	void Free(size_t instanceIndex)
	{
		static_assert(
			std::is_same<T, ElemT>::value,
			"Invalid type passed to Free(instance). The field types must be the _exact_ same.");
		MemberBufferTraits::FreeItem<T>(std::get<MemberIndex>((tuple_type&)*this), instanceIndex);
	}

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	void Free()
	{
		static_assert(
			std::is_same<T, ElemT>::value,
			"Invalid type passed to Free. The field types must be the _exact_ same.");
		MemberBufferTraits::Free<T>(std::get<MemberIndex>((tuple_type&)*this), Capacity);
	}

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	T& Get(size_t instanceIndex) const
	{
		static_assert(
			std::is_same<
				T, ElemT
			>::value,
			"Invalid type passed to NewInstance. The field types must be the _exact_ same.");
		return *MemberBufferTraits::Get<T>(std::get<MemberIndex>(*this), instanceIndex);
	}

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	void Set(size_t instanceIndex, const T& value)
	{
		static_assert(
			std::is_same<
				T, ElemT
			>::value,
			"Invalid type passed to NewInstance. The field types must be the _exact_ same.");
		MemberBufferTraits::Copy<T>(std::get<MemberIndex>(*this), instanceIndex, &value);
	}

	template<int MemberIndex, class T, typename ElemT = std::remove_pointer_t<std::tuple_element_t<MemberIndex, tuple_type>>>
	void Move(size_t instanceIndex, T&& value)
	{
		static_assert(
			std::is_same<
				T, ElemT
			>::value,
			"Invalid type passed to NewInstance. The field types must be the _exact_ same.");
		MemberBufferTraits::Move<T>(std::get<MemberIndex>(*this), instanceIndex, std::move(value));
	}
};



CLOSE_NAMESPACE(Firestorm);
#endif
