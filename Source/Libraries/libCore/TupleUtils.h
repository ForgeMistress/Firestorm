#ifndef LIBCORE_TUPLEUTILS_H_
#define LIBCORE_TUPLEUTILS_H_
#pragma once

#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

OPEN_NAMESPACE(Firestorm);





// https://en.cppreference.com/w/cpp/utility/functional/invoke#Possible_implementation
namespace detail {
template <class T>
struct is_reference_wrapper : std::false_type {};
template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
template <class T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

template <class T, class Type, class T1, class... Args>
decltype(auto) INVOKE(Type T::* f, T1&& t1, Args&&... args)
{
	if constexpr (std::is_member_function_pointer_v<decltype(f)>) {
		if constexpr (std::is_base_of_v<T, std::decay_t<T1>>)
			return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
		else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>)
			return (t1.get().*f)(std::forward<Args>(args)...);
		else
			return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
	} else {
		static_assert(std::is_member_object_pointer_v<decltype(f)>);
		static_assert(sizeof...(args) == 0);
		if constexpr (std::is_base_of_v<T, std::decay_t<T1>>)
			return std::forward<T1>(t1).*f;
		else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>)
			return t1.get().*f;
		else
			return (*std::forward<T1>(t1)).*f;
	}
}

template <class F, class... Args>
decltype(auto) INVOKE(F&& f, Args&&... args)
{
	return std::forward<F>(f)(std::forward<Args>(args)...);
}
} // namespace detail

template< class F, class... Args>
std::invoke_result_t<F, Args...> invoke(F&& f, Args&&... args) 
noexcept(std::is_nothrow_invocable_v<F, Args...>)
{
	return detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}




// https://stackoverflow.com/questions/1198260/iterate-over-tuple

/*template<
	size_t Index = 0, // start iteration at 0 index
	typename TTuple,  // the tuple type
	size_t Size =
	std::tuple_size_v<
	std::remove_reference_t<TTuple>>, // tuple size
	typename TCallable, // the callable to bo invoked for each tuple item
	typename... TArgs   // other arguments to be passed to the callable 
	>
	void tuple_for_each(TTuple&& tuple, TCallable&& callable, TArgs&&... args)
{
	if constexpr (Index < Size)
	{
		invoke(callable, std::get<Index>(tuple), args...);

		if constexpr (Index + 1 < Size)
			tuple_for_each<Index + 1>(
			std::forward<TTuple>(tuple),
			std::forward<TCallable>(callable),
			std::forward<TArgs>(args)...);
	}
}*/


/*template <typename Tup, typename R, typename F, std::size_t... Idxs>
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
*/

CLOSE_NAMESPACE(Firestorm);

#endif
