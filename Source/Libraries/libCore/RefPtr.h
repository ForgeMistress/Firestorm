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

OPEN_NAMESPACE(Firestorm);

#define REFPTR_TYPECHECK( BASE, SUPER )        \
static_assert(								   \
	std::is_base_of<BASE, SUPER>::value ||	   \
	std::is_same<BASE, SUPER>::value, 		   \
	"invalid types passed");


template<class T>
class RefPtr final
{
	template <class T> friend class WeakPtr;
public:
	using DeleteExpr = std::function<void(T*)>;

private:
	DeleteExpr _deleter{ nullptr };
	PtrControlBlock* _ctrl{ nullptr };

public:
	using element_type = T;
	using pointer_type = T*;
	using ctrl_type = PtrControlBlock;
	using deleter_type = std::function<void(T*)>;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Same Type Constructors
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// default
	RefPtr()
	: _ctrl(ctrl_type::make_block(nullptr))
	, _deleter(nullptr)
	{
	}

	// nullptr
	RefPtr(std::nullptr_t)
	: _ctrl(ctrl_type::make_block(nullptr))
	, _deleter(nullptr)
	{
	}

	// raw pointer with no deleter
	template<class Ptr_t>
	RefPtr(Ptr_t* object)
	: _ctrl(ctrl_type::make_block(object))
	, _deleter(nullptr)
	{
		REFPTR_TYPECHECK(T, Ptr_t);
		add_ref();
	}

	// raw pointer with specified deleter
	template<class Ptr_t, class Deleter>
	RefPtr(Ptr_t* object, Deleter deleter)
	: _ctrl(ctrl_type::make_block<Ptr_t>(object))
	, _deleter(deleter)
	{
		REFPTR_TYPECHECK(T, Ptr_t);
		add_ref();
	}

	// copy
	template <class U>
	RefPtr(const RefPtr<U>& other)
		: _ctrl(other.get_ctrl())
	{
		REFPTR_TYPECHECK(T, U);
		auto otherDel = other.get_deleter();
		if (otherDel)
			set_deleter(otherDel);

		add_ref();
	}

	// copy with provided deleter
	template <class U, class Del>
	RefPtr(const RefPtr<U>& other, Del deleter)
	: _ctrl(other.get_ctrl())
	, _deleter(deleter)
	{
		REFPTR_TYPECHECK(T, U);
		add_ref();
	}

	// move
	template<class U>
	RefPtr(RefPtr<U>&& other)
	: _ctrl(other.get_ctrl())
	, _deleter(other.get_deleter())
	{
		REFPTR_TYPECHECK(T, U);
		other.set_ctrl(nullptr);
		other.set_deleter(nullptr);
	}

	~RefPtr()
	{
		del_ref();
	}

	template<class U>
	RefPtr<T>& operator=(const RefPtr<U>& other)
	{
		REFPTR_TYPECHECK(T, U);
		if (this != &other)
		{
			// decrement the strong count of the existing counter.
			del_ref();

			// copy over the stuff.
			_ctrl = other.get_ctrl();
			_deleter = other.get_deleter();

			// add a reference for the new counter.
			add_ref();
		}
		return *this;
	}

	template<class U>
	RefPtr<T>& operator=(RefPtr<U>&& other)
	{
		REFPTR_TYPECHECK(T, U);
		if (this != &other)
		{
			// decrement the strong count of the existing counter.
			del_ref();

			// copy over the stuff.
			_ctrl = other.get_ctrl();
			_deleter = other.get_deleter();

			other.set_ctrl(nullptr);
			other.set_deleter(nullptr);
		}
		return *this;
	}

	RefPtr<T>& operator=(std::nullptr_t)
	{
		del_ref();

		// _ctrl->set_ptr(nullptr);
		_deleter = nullptr;
		return *this;
	}

	template<class U>
	RefPtr<U> Upcast() const
	{
		static_assert(
			std::is_base_of<T, U>::value &&
			!std::is_same<T, U>::value,
			"invalid types passed");

		if(_ctrl == nullptr)
			return nullptr;

		RefPtr<U> out(get_ctrl());
		auto deleter = get_deleter();
		if(deleter)
		{
			out.set_deleter([del = deleter](U* ptr) {
				// the original will still work, but the output RefPtr deleter needs to have the signature of the
				// proper type.
				del(ptr);
			});
		}

		return out;
	}

	T* Get() const { return _ctrl->get_ptr<T>(); }

	T* operator->() { return _ctrl->get_ptr<T>(); }
	const T* operator->() const { return _ctrl->get_ptr<T>(); }

	template<class U>
	bool operator==(const RefPtr<U>& other) const
	{
		REFPTR_TYPECHECK(T, RefPtr<U>::element_type);
		return _ctrl == other.get_ctrl();
	}

	bool operator==(std::nullptr_t) const
	{
		return _ctrl && _ctrl->get_ptr<T>() == nullptr;
	}

	template<class U>
	bool operator!=(const RefPtr<U>& other) const
	{
		REFPTR_TYPECHECK(T, RefPtr<U>::element_type);
		return _ctrl != other.get_ctrl();
	}

	bool operator!=(std::nullptr_t) const
	{
		if (_ctrl)
		{
			auto ptr = _ctrl->get_ptr<T>();
			return ptr != nullptr;
		}
		return true;
		//return _ctrl && _ctrl->get_ptr<T>() != nullptr;
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

	void set_deleter(deleter_type deleter)
	{
		if (deleter)
			_deleter = deleter;
	}

	template<class Deleter>
	void set_deleter(Deleter deleter)
	{
		_deleter = deleter;
	}

	void set_deleter(std::nullptr_t)
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
CLOSE_NAMESPACE(std);

#endif