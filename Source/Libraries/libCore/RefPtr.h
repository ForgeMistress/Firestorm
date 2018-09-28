///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  RefPtr
//
//  Reference counted smart pointer.
//  Adapted from http://www.acodersjourney.com/implementing-smart-pointer-using-reference-counting/\
//  With some tidbits from https://codereview.stackexchange.com/questions/99087/alternate-weak-pointer-implementation
//
//  Season to taste.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_REFPTR_H_
#define LIBCORE_REFPTR_H_
#pragma once

#include "libCore.h"
#include "Assert.h"
#include "PtrControlBlock.h"
#include <functional>
#include <rttr/wrapper_mapper.h>
#include <memory>

OPEN_NAMESPACE(Firestorm);

/*#define REFPTR_TYPECHECK( BASE, SUPER )        \
static_assert(								   \
	std::is_base_of<BASE, SUPER>::value ||	   \
	std::is_same<BASE, SUPER>::value, 		   \
	"invalid types passed");

OPEN_NAMESPACE(detail);

template<class Base, class Derived>
struct is_same_or_base
{
	static constexpr bool value =
		std::is_same<Base, Derived>::value ||
		std::is_base_of<Base, Derived>::value;
};

template<class T, typename PointerT = typename std::remove_pointer<T>::type>
struct deleter_sig
{
	using arg_type = PointerT*;
	using func_type = std::function<void(arg_type)>;
	using element_type = PointerT;
};

struct base_tag {};
struct same_tag {};
struct conv_tag {};

template<class Base, class Derived, typename t>
struct relation_tag_base
{
	using tag = t;
};

CLOSE_NAMESPACE(detail);

template<class T>
class RefPtr final
{
	template <class T> friend class WeakPtr;
public:
	using DeleteExpr = typename detail::deleter_sig<T>::func_type;

private:
	DeleteExpr _deleter{ nullptr };
	PtrControlBlock* _ctrl{ nullptr };

public:
	using element_type = T;
	using pointer_type = T*;
	using ctrl_type = PtrControlBlock;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Same Type Constructors
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// default
	RefPtr()
	: _ctrl(ctrl_type::make_block(nullptr))
	{
		set_deleter(nullptr);
	}

	// nullptr
	RefPtr(std::nullptr_t)
	: _ctrl(ctrl_type::make_block(nullptr))
	{
		set_deleter(nullptr);
	}

	// raw pointer with no deleter
	template<class Ptr_t>
	RefPtr(Ptr_t* object)
	: _ctrl(ctrl_type::make_block(object))
	{
		REFPTR_TYPECHECK(T, Ptr_t);
		set_deleter(nullptr);
		add_ref();
	}

	// raw pointer with specified deleter
	template<class Ptr_t, class Deleter>
	RefPtr(Ptr_t* object, Deleter deleter)
	: _ctrl(ctrl_type::make_block<Ptr_t>(object))
	{
		REFPTR_TYPECHECK(T, Ptr_t);
		set_deleter(deleter);
		add_ref();
	}

	// copy
	RefPtr(const RefPtr<T>& other)
	: _ctrl(other.get_ctrl())
	{
		set_deleter(other.get_deleter());

		add_ref();
	}

	template<class U>
	RefPtr(const RefPtr<U>& other)
	: _ctrl(other.get_ctrl())
	{
		REFPTR_TYPECHECK(T, U);
		if(other.get_deleter())
			set_deleter([del = other.get_deleter()](T* ptr){
				if(del) del(reinterpret_cast<U*>(ptr));
			});
		add_ref();
	}

	// move
	RefPtr(RefPtr<T>&& other)
	: _ctrl(other.get_ctrl())
	{
		set_deleter(other.get_deleter());

		other.set_ctrl(nullptr);
		other.set_deleter(nullptr);
	}

	~RefPtr()
	{
		del_ref();
	}

	RefPtr<T>& operator=(const RefPtr<T>& other)
	{
		if(this != &other)
		{
			// decrement the strong count of the existing counter.
			del_ref();

			// copy over the stuff.
			set_ctrl(other.get_ctrl());
			set_deleter(other.get_deleter());

			// add a reference for the new counter.
			add_ref();
		}
		return *this;
	}

	RefPtr<T>& operator=(RefPtr<T>&& other)
	{
		if(this != &other)
		{
			// decrement the strong count of the existing counter.
			del_ref();

			// move over the stuff.
			set_ctrl(other.get_ctrl());
			set_deleter(other.get_deleter());

			// take ownership
			other.set_ctrl(nullptr);
			other.set_deleter(nullptr);
		}
		return *this;
	}

	RefPtr<T>& operator=(std::nullptr_t)
	{
		del_ref();
		set_deleter(nullptr);
		return *this;
	}

	template<class U>
	RefPtr<U> Upcast() const
	{
		REFPTR_TYPECHECK(T, U);

		if(_ctrl == nullptr)
			return nullptr;

		return RefPtr<U>(get_ctrl(), get_deleter());
	}

	T* Get() const { return _ctrl->get_ptr<T>(); }

	T* operator->() { return _ctrl->get_ptr<T>(); }
	const T* operator->() const { return _ctrl->get_ptr<T>(); }

	bool operator==(const RefPtr<T>& other) const
	{
		return _ctrl == other.get_ctrl();
	}

	bool operator==(std::nullptr_t) const
	{
		return _ctrl && _ctrl->get_ptr<T>() == nullptr;
	}

	bool operator!=(const RefPtr<T>& other) const
	{
		return _ctrl != other.get_ctrl();
	}

	bool operator!=(std::nullptr_t) const
	{
		if(_ctrl)
		{
			auto ptr = _ctrl->get_ptr<T>();
			return ptr != nullptr;
		}
		return true;
	}

	operator bool() const
	{
		return _ctrl != nullptr;
	}

	size_t GetCount() const
	{
		return _ctrl ? _ctrl->get_strong_count() : 0;
	}

	size_t GetWeakCount() const
	{
		return _ctrl ? _ctrl->get_weak_count() : 0;
	}

	PtrControlBlock* get_ctrl() const
	{
		return _ctrl;
	}

	void set_ctrl(PtrControlBlock* ctrl)
	{
		_ctrl = ctrl;
	}

	DeleteExpr get_deleter() const
	{
		return _deleter;
	}

	// default
	template<class Deleter=DeleteExpr>
	void set_deleter(Deleter deleter)
	{
		if(_deleter)
			_deleter = deleter;
	}

	// nullptr set
	template<>
	void set_deleter<std::nullptr_t>(std::nullptr_t)
	{
		_deleter = nullptr;
	}

	// used by the WeakPtr and Upcast
	RefPtr(PtrControlBlock* ctrl)
	: _ctrl(ctrl)
	, _deleter(nullptr)
	{
		FIRE_ASSERT_MSG(_ctrl != nullptr, "the control block passed to RefPtr was nullptr");
		add_ref();
	}

	void add_ref()
	{
		if(_ctrl)
			_ctrl->add_strong_ref();
	}

	void del_ref()
	{
		if(_ctrl)
		{
			_ctrl->del_strong_ref();

			if(_ctrl->get_strong_count() == 0)
			{
				_ctrl->delete_ptr<T>(get_deleter());
				_ctrl->set_ptr<T>(nullptr);

				if(_ctrl->get_weak_count() == 0)
				{
					ctrl_type::return_block(_ctrl);
					_ctrl = nullptr;
				}
			}
		}
	}
};

CLOSE_NAMESPACE(Firestorm);

OPEN_NAMESPACE(rttr);

template<typename T>
struct wrapper_mapper<Firestorm::RefPtr<T>>
{
	using wrapped_type = decltype(Firestorm::RefPtr<T>().Get());
	using type = Firestorm::RefPtr<T>;

	static RTTR_INLINE wrapped_type get(const type& obj)
	{
		return obj.Get();
	}

	static RTTR_INLINE type create(const wrapped_type& t)
	{
		return type(t);
	}

	template<typename U>
	static Firestorm::RefPtr<U> convert(const type& source, bool& ok)
	{
		if (auto p = rttr_cast<typename Firestorm::RefPtr<U>::element_type*>(source.Get()))
		{
			ok = true;
			return Firestorm::RefPtr<U>(p);
		}
		else
		{
			ok = false;
			return Firestorm::RefPtr<U>(nullptr);
		}
	}
};

CLOSE_NAMESPACE(rttr);

OPEN_NAMESPACE(std);

template<class T>
struct hash<Firestorm::RefPtr<T>>
{
	size_t operator()(const Firestorm::RefPtr<T>& ptr) const
	{
		return std::hash<Firestorm::RefPtr<T>::pointer_type>()(ptr.Get());
	}
};
CLOSE_NAMESPACE(std);*/

template<class T> using RefPtr = std::shared_ptr<T>;

CLOSE_NAMESPACE(Firestorm);

#endif