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
#include <rttr/wrapper_mapper.h>

OPEN_NAMESPACE(Firestorm);

class RefCount final
{
	template <class T> friend class RefPtr;
	template <class T> friend class WeakPtr;
public:
	template <class T>
	RefCount(T* object)
		:_object(static_cast<void*>(object))
	{
	}

	template<class T>
	T* _get_ptr() const { return static_cast<T*>(_object); }

private:
	uint32_t _strongCount{ 0 };
	uint32_t _weakCount{ 0 };
	void* _object;
};

template<class T>
class RefPtr final
{
	template <class T> friend class WeakPtr;

public:
	using DeleteExpr = std::function<void(T*)>;
	typedef T element_type;
	typedef T* pointer_type;

	RefPtr()
	: _object(nullptr)
	, _deleter(nullptr)
	{
	}

	RefPtr(std::nullptr_t)
	: _object(nullptr)
	, _deleter(nullptr)
	{
	}

	RefPtr(T* object)
	: _object(object)
	, _deleter(nullptr)
	{
		_object->AddRef();
	}

	template<class Deleter>
	RefPtr(T* object, Deleter deleter)
	: _object(object)
	, _deleter(deleter)
	{
		_object->AddRef();
	}

	RefPtr(const RefPtr<T>& other)
	: _object(other._object)
	, _deleter(other._deleter)
	{
		_object->AddRef();
	}

	// polymorphic support
	template <class Subclass_t>
	RefPtr(const RefPtr<Subclass_t>& other)
		: _object(other._get_ptr())
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type that is a subclass of the held type");

		_object->AddRef();

		// This is IWhatever, other is class Whatever : public IWhatever. ya feel?
		auto otherDel = other._get_deleter();
		if (otherDel)
		{
			// what has science done?
			_deleter = [del = otherDel](T* ptr) {
				if (ptr)
					// so in order to call the other deleter what the superclass defined, we need to wrap it in our own
					// that supports our needs.
					del(reinterpret_cast<Subclass_t*>(ptr));
			};
		}
	}

	// polymorphic support with provided deleter.
	template <class Subclass_t, class Del>
	RefPtr(const RefPtr<Subclass_t>& other, Del deleter)
		: _object(other._object)
		, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		_object->AddRef();
	}

	// polymorphic support for raw pointer
	template <class Subclass_t>
	RefPtr(Subclass_t* ptr)
	: _object(static_cast<T*>(ptr))
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		_object->AddRef();
	}

	// polymorphic support for raw pointer
	template <class Subclass_t, class Del>
	RefPtr(Subclass_t* ptr, Del deleter)
	: _object(static_cast<T*>(ptr))
	, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		_object->AddRef();
	}

	// polymorphic move
	template<class Subclass_t>
	RefPtr(RefPtr<Subclass_t>&& other)
	: _object(other._object)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting move constructor of RefPtr must hold a type "
			"that is a subclass of the held type");

		// This is IWhatever, other is class Whatever : public IWhatever. ya feel?
		auto otherDel = other._get_deleter();
		if (otherDel)
		{
			// what has science done?
			_deleter = [del = otherDel](T* ptr) {
				if (ptr)
					// so in order to call the other deleter what the superclass defined, we need to wrap it in our own
					// that supports our needs.
					del(reinterpret_cast<Subclass_t*>(ptr));
			};
		}

		// assumes ownership
		other._object = nullptr;
		other._deleter = nullptr;
	}

	RefPtr(RefPtr<T>&& other)
	: _object(other._object)
	{
		_object = std::move(other._object);
		_deleter = std::move(other._deleter);

		// assumes ownership
		other._object = nullptr;
		other._deleter = nullptr;
	}

	~RefPtr()
	{
		if(_object)
			_object->DelRef();
		DoCleanup();
	}

	RefPtr<T>& operator=(const RefPtr<T>& other)
	{
		if(this != &other)
		{
			// decrement the strong count of the existing counter.
			if (_object)
				_object->DelRef();
			DoCleanup();
			// any weak pointers that still hold the control block from this RefPtr will clean them up as
			// expected.

			// copy over the stuff.
			_object = other._object;
			_deleter = other._deleter;

			// add a reference for the new counter.
			if(_object)
				_object->AddRef();
		}
		return *this;
	}

	RefPtr<T>& operator=(std::nullptr_t)
	{
		if(_object)
			_object->DelRef();
		_object = nullptr;
		_deleter = nullptr;
		return *this;
	}

	RefPtr<T>& operator=(RefPtr<T>&& other)
	{
		if(this != &other)
		{
			if(_object)
				_object->DelRef();

			_object = other._object; // we're moving it, so no increment to the ref count.

			other._object = nullptr;
			other._deleter = nullptr;
		}
		return *this;
	}

	template<class U>
	RefPtr<U> Upcast()
	{
		if(_object == nullptr)
			return nullptr;

		RefPtr<U> out(static_cast<U*>(_object));
		if (out._get_deleter())
			out._set_deleter([del = _deleter](U* ptr) {
			// the original will still work, but the output RefPtr deleter needs to have the signature of the
			// proper type.
			del(ptr);
		});
		// increment the ref count.
		return out;
	}

	T* Get() const { return _object; }
	T& operator*() { return *_object; }
	T* operator->() { return _object; }

	const T* operator->() const { return _object; }

	bool operator==(const RefPtr<T>& other) const
	{
		return _object == other._object;
	}

	bool operator==(std::nullptr_t) const
	{
		return _object == nullptr;
	}

	bool operator!=(const RefPtr<T>& other) const
	{
		return _object != other._object;
	}

	bool operator!=(std::nullptr_t) const
	{
		return _object != nullptr;
	}

	operator bool() const
	{
		return _object != nullptr;
	}

	uint32_t GetCount() const
	{
		return _object ? _object->_refCount : 0;
	}

	template<class U = T>
	U*         _get_ptr()     const { return static_cast<U*>(_object); }
	DeleteExpr _get_deleter() const { return _deleter; }

	template<class U = T>
	void _set_ptr(U* ptr) { _object = ptr; }
	void _set_count(RefCount* count) { _count = count; }
	template<class Del>
	void _set_deleter(Del del) { _deleter = del; }

private:
	void DoCleanup()
	{
		// if there are no more strong references...
		if(_object && _object->_refCount == 0)
		{
			// then delete the object.
			if (_deleter) _deleter(_object);
			else delete _object;

			// and null out the counter's pointer as well.
			_object = nullptr;
		}
	}

	T*         _object{ nullptr };
	DeleteExpr _deleter{ nullptr };
};

template <class T>
class WeakPtr final
{
public:
	typedef T* T_ptr;

	WeakPtr() : _object(nullptr) {}
	WeakPtr(std::nullptr_t) : _object(nullptr) {}
	WeakPtr(const RefPtr<T>& ptr)
		: _object(ptr._object)
	{
	}

	WeakPtr(const WeakPtr<T>& ptr)
		: _object(ptr._object)
	{
	}

	template <class Subclass_t>
	WeakPtr(const RefPtr<Subclass_t>& ptr)
		: _object(ptr._object)
	{
		FIRE_ASSERT(_count);
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the pointer passed to the casting constructor must be a subclass of the type held by the WeakPtr");
	}

	bool operator==(WeakPtr<T>& other)
	{
		return other._object == _object;
	}

	bool operator==(RefPtr<T>& other)
	{
		return other._object == _object;
	}

	bool operator==(std::nullptr_t) const
	{
		return _object && _object == nullptr;
	}

	bool operator!=(std::nullptr_t) const
	{
		return _object && _object != nullptr;
	}

	template <class Subclass_t>
	WeakPtr<T>& operator=(const RefPtr<Subclass_t>& ptr)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor must hold an instance of a subclass of the type held by the WeakPtr");

		_object = static_cast<T*>(ptr._object);
		return *this;
	}

	WeakPtr<T>& operator=(const RefPtr<T>& obj)
	{
		_object = obj._object;
		return *this;
	}

	RefPtr<T> Lock() const
	{
		return RefPtr<T>(_object);
	}

	operator bool() const
	{
		return _object != nullptr;
	}
private:
	T* _object;
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