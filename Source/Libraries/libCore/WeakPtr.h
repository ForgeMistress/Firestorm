///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PtrControlBlock
//
//  Control block for RefPtr and WeakPtr.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_WEAKPTR_H_
#define LIBCORE_WEAKPTR_H_
#pragma once

//#include "RefPtr.h"
//#include "PtrControlBlock.h"
#include <memory>

OPEN_NAMESPACE(Firestorm);

/*template <class T>
class WeakPtr final
{
public:
	using ctrl_type = PtrControlBlock;

	WeakPtr()
	: _ctrl(nullptr)
	{
	}

	WeakPtr(std::nullptr_t)
	: _ctrl(nullptr)
	{
	}

	WeakPtr(const RefPtr<T>& ptr)
	: _ctrl(ptr.get_ctrl())
	{
		add_ref();
	}

	WeakPtr(const WeakPtr<T>& ptr)
	: _ctrl(ptr._ctrl)
	{
		add_ref();
	}

	template <class Subclass_t>
	WeakPtr(const RefPtr<Subclass_t>& ptr)
	: _ctrl(ptr.get_control())
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the pointer passed to the casting constructor must be a subclass of the type held by the WeakPtr");
		add_ref();
	}

	~WeakPtr()
	{
		del_ref();
	}

	bool operator==(WeakPtr<T>& other)
	{
		return _ctrl && _ctrl->get_ptr<T>() == other._ctrl->get_ptr<T>();
	}

	bool operator==(RefPtr<T>& other)
	{
		return _ctrl && _ctrl->get_ptr<T>() == other.Get();
	}

	bool operator==(std::nullptr_t) const
	{
		return _ctrl && _ctrl->get_ptr<T>() == nullptr;
	}

	bool operator!=(std::nullptr_t) const
	{
		return _ctrl && _ctrl->get_ptr<T>() != nullptr;
	}

	template <class Subclass_t>
	WeakPtr<T>& operator=(const RefPtr<Subclass_t>& ptr)
	{
		static_assert(std::is_base_of<T, Subclass_t>::value,
			"the RefPtr passed to the casting constructor must hold an instance of a subclass of the type held by the WeakPtr");

		_ctrl = ptr.get_ctrl();

		add_ref();

		return *this;
	}

	WeakPtr<T>& operator=(const RefPtr<T>& ptr)
	{
		del_ref();

		_ctrl = ptr.get_ctrl();

		add_ref();
		return *this;
	}

	RefPtr<T> Lock() const
	{
		return RefPtr<T>(_ctrl);
	}

	operator bool() const
	{
		return _ctrl && _ctrl->get_ptr<T>() != nullptr;
	}

	PtrControlBlock* get_ctrl() const
	{
		return _ctrl;
	}
private:
	void add_ref()
	{
		if(_ctrl)
			_ctrl->add_weak_ref();
	}

	void del_ref()
	{
		if(_ctrl)
		{
			_ctrl->del_weak_ref();
			if (_ctrl->get_weak_count() <= 0 && _ctrl->get_strong_count() <= 0)
				ctrl_type::return_block(_ctrl);
		}
	}
	PtrControlBlock* _ctrl{ nullptr };
};*/

CLOSE_NAMESPACE(Firestorm);
#endif