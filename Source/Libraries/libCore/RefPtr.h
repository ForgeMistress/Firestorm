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

#include <libCore/libCore.h>
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

// Dummy type that allows the program to compile when the static_assert hits in RefPtr.
struct _RefPtr_DummyType { virtual ~_RefPtr_DummyType() {} };

template<class T>
class RefPtr final
{
	template <class T> friend class WeakPtr;
	/*static_assert(std::has_virtual_destructor<
		std::conditional<std::is_polymorphic<T>::value, T, _RefPtr_DummyType>::type
	>::value, "Objects used in a RefPtr must have a virtual destructor");*/
	
public:
	using DeleteExpr = std::function<void(T*)>;
	typedef T element_type;
	typedef T* pointer_type;

	RefPtr()
	: _object(nullptr)
	, _count(nullptr)
	, _deleter(nullptr)
	{
	}

	RefPtr(std::nullptr_t)
	: _object(nullptr)
	, _count(nullptr) 
	, _deleter(nullptr)
	{
	}

	RefPtr(T* object)
	: _object(object)
	, _count(new RefCount(object))
	, _deleter(nullptr)
	{
		assert(_count && _count->_object == _object);
		++_count->_strongCount;
	}

	template<class Deleter>
	RefPtr(T* object, Deleter deleter)
	: _object(object)
	, _count(new RefCount(object))
	, _deleter(deleter)
	{
		assert(_count && _count->_object == _object);
		++_count->_strongCount;
	}

	RefPtr(const RefPtr<T>& other)
	: _object(other._object)
	, _count(other._count)
	, _deleter(other._deleter)
	{
		assert(_count && _count->_object == _object);
		++_count->_strongCount;
	}

	// polymorphic support
	template <class Subclass_t>
	RefPtr(const RefPtr<Subclass_t>& other)
	: _object(other._get_ptr())
	, _count(other._get_count())
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type that is a subclass of the held type");

		++_count->_strongCount;

		// This is IWhatever, other is class Whatever : public IWhatever. ya feel?
		auto otherDel = other._get_deleter();
		if(otherDel)
		{
			// what has science done?
			_deleter = [del = otherDel](T* ptr) {
				if(ptr)
					// so in order to call the other deleter what the superclass defined, we need to wrap it in our own
					// that supports our needs.
					del(reinterpret_cast<Subclass_t*>(ptr));
			};
		}
	}

	// polymorphic support with provided deleter.
	template <class Subclass_t, class Del>
	RefPtr(const RefPtr<Subclass_t>& other, Del deleter)
	: _object(other._get_ptr())
	, _count(other._get_count())
	, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		++_count->_strongCount;
	}

	// polymorphic support for raw pointer
	template <class Subclass_t>
	RefPtr(Subclass_t* ptr)
	: _object(static_cast<T*>(ptr))
	, _count(new RefCount(_object))
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		++_count->_strongCount;
	}

	// polymorphic support for raw pointer
	template <class Subclass_t, class Del>
	RefPtr(Subclass_t* ptr, Del deleter)
	: _object(static_cast<T*>(ptr))
	, _count(new RefCount(_object))
	, _deleter(deleter)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		++_count->_strongCount;
	}

	// polymorphic move
	// when passing in the 
	template<class Subclass_t>
	RefPtr(RefPtr<Subclass_t>&& other)
	: _object(other._object)
	, _count(other._count)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting move constructor of RefPtr must hold a type "
			"that is a subclass of the held type");
		assert(_count && _count->_object == _object);

		// This is IWhatever, other is class Whatever : public IWhatever. ya feel?
		auto otherDel = other._get_deleter();
		if(otherDel)
		{
			// what has science done?
			_deleter = [del = otherDel](T* ptr) {
				if(ptr)
					// so in order to call the other deleter what the superclass defined, we need to wrap it in our own
					// that supports our needs.
					del(reinterpret_cast<Subclass_t*>(ptr));
			};
		}

		// assumes ownership
		other._count = nullptr;
		other._object = nullptr;
		other._deleter = nullptr;
	}

	RefPtr(RefPtr<T>&& other)
	: _object(other._object)
	, _count(other._count)
	, _deleter(other._deleter)
	{
		assert(_count && "counter block was nullptr");
		T* o = _count->_get_ptr<T>();
		assert(o == _object && "counter block held a different object for some reason");

		// assumes ownership
		other._count = nullptr;
		other._object = nullptr;
		other._deleter = nullptr;
	}

	~RefPtr()
	{
		if(_count)
			--_count->_strongCount;
		DoCleanup();
	}

	RefPtr<T>& operator=(const RefPtr<T>& other)
	{
		if(this != &other)
		{
			// decrement the strong count of the existing counter.
			if(_count)
				--_count->_strongCount;
			DoCleanup();
			// any weak pointers that still hold the control block from this RefPtr will clean them up as
			// expected.

			// copy over the stuff.
			_object = other._object;
			_count = other._count;
			_deleter = other._deleter;

			// add a reference for the new counter.
			assert(_count);
			++_count->_strongCount;
		}
		return *this;
	}

	RefPtr<T>& operator=(RefPtr<T>&& other)
	{
		if(this != &other)
		{
			_object = other._object;
			_count = other._count;

			assert(_count && _count->_object == other._object);

			other._count = nullptr;
			other._object = nullptr;
			other._deleter = nullptr;
		}
		return *this;
	}

	template<class U>
	RefPtr<U> Upcast()
	{
		FIRE_ASSERT(_object != nullptr && _count != nullptr);
		RefPtr<U> out(nullptr);

		out._set_ptr(static_cast<U*>(_object));
		out._set_count(_count);
		if(out._get_deleter())
			out._set_deleter([del = _deleter](U* ptr) {
				// the original will still work, but the output RefPtr deleter needs to have the signature of the
				// proper type.
				del(ptr);
			});
		// increment the ref count.
		++out._get_count()->_strongCount;
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
		FIRE_ASSERT(_count);
		return _count->_strongCount;
	}

	uint32_t GetWeakCount() const
	{
		FIRE_ASSERT(_count);
		return _count->_weakCount;
	}

	template<class U=T>
	U*         _get_ptr()     const { return static_cast<U*>(_object); }
	RefCount*  _get_count()   const { return _count; }
	DeleteExpr _get_deleter() const { return _deleter; }

	template<class U=T>
	void _set_ptr(U* ptr) { _object = ptr; }
	void _set_count(RefCount* count) { _count = count; }
	template<class Del>
	void _set_deleter(Del del) { _deleter = del; }

private:
	RefPtr(RefCount* refCount)
	: _object(static_cast<T*>(refCount->_object))
	, _count(refCount)
	{
		FIRE_ASSERT(_count);
		++_count->_strongCount;
	}

	void DoCleanup()
	{
		// if there are no more strong references...
		if (_count && _count->_strongCount == 0)
		{
			// then delete the object.
			// _object->~T(); // call the destructor so that we can delete it properly.
			// delete _object;
			// _object = nullptr;
			if(_deleter) _deleter(_object);
			else delete _object;

			// and null out the counter's pointer as well.
			_count->_object = nullptr;

			// if there are no more weak references, then we can delete the counter as well.
			if(_count->_weakCount == 0)
			{
				delete _count;
				_count = nullptr;
			}
		}
	}

	T*         _object{ nullptr };
	RefCount*  _count{ nullptr };
	DeleteExpr _deleter{ nullptr };
};

template <class T>
class WeakPtr final
{
public:
	typedef T* T_ptr;

	WeakPtr() : _count(nullptr) {}
	WeakPtr(const RefPtr<T>& ptr)
	: _count(ptr._count)
	{
		FIRE_ASSERT(_count);
		++_count->_weakCount;
	}

	WeakPtr(const WeakPtr<T>& ptr)
	: _count(ptr._count)
	{
		FIRE_ASSERT(_count);
		++_count->_weakCount;
	}

	template <class Subclass_t>
	WeakPtr(const RefPtr<Subclass_t>& ptr)
	: _count(ptr._count)
	{
		FIRE_ASSERT(_count);
		static_assert(std::is_base_of<T, Subclass_t>::value, 
			"the pointer passed to the casting constructor must be a subclass of the type held by the WeakPtr");
		++_count->_weakCount;
	}

	~WeakPtr()
	{
		if(_count)
		{
			// if there are no more weak references and there's no object (released by the RefPtr)
			// then we can delete the control block.
			if(--_count->_weakCount == 0 && !_count->_object)
			{
				delete _count;
				_count = nullptr;
			}
		}
	}

	bool operator==(WeakPtr<T>& other)
	{
		return other._count == _count;
	}

	bool operator==(RefPtr<T>& other)
	{
		return other._count == _count;
	}

	bool operator==(std::nullptr_t) const
	{
		return _counter && _counter->_object == nullptr;
	}

	bool operator!=(std::nullptr_t) const
	{
		return _counter && _counter->_object != nullptr;
	}

	template <class Subclass_t>
	WeakPtr<T>& operator=(const RefPtr<Subclass_t>& ptr)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor must hold an instance of a subclass of the type held by the WeakPtr");

		// decrement the weak ref count of the current control block if we have it
		// and then clean it up if we need to.
		if(_count)
		{
			if(--_count->_weakCount == 0 && !_count->_object)
			{
				delete _count;
				_count = nullptr;
			}
		}

		// increment the weak ref count of the new control block.
		_count = ptr._count;
		FIRE_ASSERT(_count);
		++_count->_weakCount;
		return *this;
	}

	WeakPtr<T>& operator=(const RefPtr<T>& obj)
	{ 
		// decrement the weak ref count of the current control block if we have it
		// and then clean it up if we need to.
		if(_count)
		{
			if(--_count->_weakCount == 0 && !_count->_object)
			{
				delete _count;
				_count = nullptr;
			}
		}

		// increment the weak ref count of the new control block.
		_count = obj._count;
		FIRE_ASSERT(_count);
		++_count->_weakCount;
		return *this; 
	}

	RefPtr<T> Lock() const
	{
		return RefPtr<T>(_count);
	}

	operator bool() const
	{
		return _count->_object != nullptr;
	}
private:
	RefCount* _count;
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
		if(auto p = rttr_cast<typename Firestorm::RefPtr<U>::element_type*>(source.Get()))
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