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

class RefPtrBase
{
	template <class T> friend class WeakPtr;
public:
	template <class T>
	RefPtrBase(T* object)
	: _object(static_cast<void*>(object))
	{
	}

	template<class T>
	T* get_ptr() const { return static_cast<T*>(_object); }

	template<class T>
	void set_ptr(T* ptr)
	{
		_object = ptr;
	}

	template<class T, class Del>
	void delete_ptr(Del deleter)
	{
		// then delete the object.
		if(deleter) deleter(static_cast<T*>(_object));
		else delete static_cast<T*>(_object);
	}

private:
	void* _object{ nullptr };
};

template<class T>
class RefPtr : public RefPtrBase
{
	template <class T> friend class WeakPtr;

public:
	using DeleteExpr = std::function<void(T*)>;
	typedef T element_type;
	typedef T* pointer_type;

	RefPtr()
	, _deleter(nullptr)
	{
	}

	RefPtr(std::nullptr_t)
	, _deleter(nullptr)
	{
	}

	RefPtr(T* object)
	: RefPtrBase(object)
	, _deleter(nullptr)
	{
		_object->AddRef();
	}

	template<class Deleter>
	RefPtr(T* object, Deleter deleter)
	: RefPtrBase(object)
	, _deleter(deleter)
	{
		get_ptr<T>()->AddRef();
	}

	RefPtr(const RefPtr<T>& other)
	: RefPtrBase(other._object)
	, _deleter(other._deleter)
	{
		get_ptr<T>()->AddRef();
	}

	// polymorphic support
	template <class Subclass_t>
	RefPtr(const RefPtr<Subclass_t>& other)
		: RefPtrBase(other._get_ptr())
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type that is a subclass of the held type");

		get_ptr<T>()->AddRef();

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
		: RefPtrBase(other._object)
		, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		get_ptr<T>()->AddRef();
	}

	// polymorphic support for raw pointer
	template <class Subclass_t>
	RefPtr(Subclass_t* ptr)
	: RefPtrBase(static_cast<T*>(ptr))
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		get_ptr<T>()->AddRef();
	}

	// polymorphic support for raw pointer
	template <class Subclass_t, class Del>
	RefPtr(Subclass_t* ptr, Del deleter)
	: RefPtrBase(static_cast<T*>(ptr))
	, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		get_ptr<T>()->AddRef();
	}

	// polymorphic move
	template<class Subclass_t>
	RefPtr(RefPtr<Subclass_t>&& other)
	: RefPtrBase(other._object)
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
	: RefPtrBase(other._object)
	{
		_object = std::move(other._object);
		_deleter = std::move(other._deleter);

		// assumes ownership
		other.set_ptr(nullptr);
		other._deleter = nullptr;
	}

	~RefPtr()
	{
		if(get_ptr<T>())
			get_ptr<T>()->DelRef();
		DoCleanup();
	}

	RefPtr<T>& operator=(const RefPtr<T>& other)
	{
		if(this != &other)
		{
			// decrement the strong count of the existing counter.
			if (get_ptr<T>())
				get_ptr<T>()->DelRef();
			DoCleanup();
			// any weak pointers that still hold the control block from this RefPtr will clean them up as
			// expected.

			// copy over the stuff.
			set_ptr<T>(other._object);
			_deleter = other._deleter;

			// add a reference for the new counter.
			if(get_ptr<T>())
				get_ptr<T>()->AddRef();
		}
		return *this;
	}

	RefPtr<T>& operator=(std::nullptr_t)
	{
		if(_object)
			_object->DelRef();
		set_ptr<T>(nullptr);
		_deleter = nullptr;
		return *this;
	}

	RefPtr<T>& operator=(RefPtr<T>&& other)
	{
		if(this != &other)
		{
			if(get_ptr<T>())
				get_ptr<T>()->DelRef();

			set_ptr<T>(other._object); // we're moving it, so no increment to the ref count.

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

	T* Get() const { return get_ptr<T>(); }
	T* operator->() { return get_ptr<T>(); }

	const T* operator->() const { return get_ptr<T>(); }

	bool operator==(const RefPtr<T>& other) const
	{
		return get_ptr<T>() == other.get_ptr<T>();
	}

	bool operator==(std::nullptr_t) const
	{
		return get_ptr<T>() == nullptr;
	}

	bool operator!=(const RefPtr<T>& other) const
	{
		return get_ptr<T>() != other._object;
	}

	bool operator!=(std::nullptr_t) const
	{
		return get_ptr<T>() != nullptr;
	}

	operator bool() const
	{
		return get_ptr<T>() != nullptr;
	}

	uint32_t GetCount() const
	{
		return get_ptr<T>() ? get_ptr<T>()->_refCount : 0;
	}

	DeleteExpr _get_deleter() const { return _deleter; }

	template<class Del>
	void _set_deleter(Del del) { _deleter = del; }

private:
	void DoCleanup()
	{
		// if there are no more strong references...
		if(get_ptr<T>() && get_ptr<T>()->_refCount == 0)
		{
			delete_ptr<T>(_deleter);

			// and null out the counter's pointer as well.
			set_ptr<T>(nullptr);
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

	WeakPtr(std::nullptr_t) : _ptr(RefPtrBase(nullptr)) {}
	WeakPtr(const RefPtrBase& ptr)
	: _ptr(ptr)
	{
	}

	WeakPtr(const WeakPtr<T>& ptr)
	: _ptr(ptr._object)
	{
	}

	template <class Subclass_t>
	WeakPtr(const RefPtr<Subclass_t>& ptr)
		: _ptr(ptr)
	{
		FIRE_ASSERT(_count);
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the pointer passed to the casting constructor must be a subclass of the type held by the WeakPtr");
	}

	bool operator==(WeakPtr<T>& other)
	{
		return other._ptr.get_ptr<T>() == _ptr.get_ptr<T>();
	}

	bool operator==(RefPtr<T>& other)
	{
		return other.get_ptr<T>() == _ptr.get_ptr<T>();
	}

	bool operator==(std::nullptr_t) const
	{
		return _ptr.get_ptr<T>() && _ptr.get_ptr<T>() == nullptr;
	}

	bool operator!=(std::nullptr_t) const
	{
		return _ptr.get_ptr<T>() && _ptr.get_ptr<T>() == nullptr;
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
	T* & _object;
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