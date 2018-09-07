///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EventDispatcher
//
//  Dispatches events to its listeners.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_EVENTDISPATCHER_H_
#define LIBMIRROR_EVENTDISPATCHER_H_
#pragma once

#include <libCore/RefPtr.h>
#include "Object.h"

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(Mirror);

class Object;

CLOSE_NAMESPACE(Mirror);

class IEvent
{
	FIRE_MIRROR_DECLARE(IEvent);
public:
	virtual const Mirror::Type& GetEventType() const = 0;
};

class EventDispatcher;

class EDReceipt
{
	friend class EventDispatcher;
	template<class T> friend class RefPtr;
private:
	EDReceipt(EventDispatcher* dispatcher, IEvent* event);
	~EDReceipt();

	void DispatcherDeleted();

private:
	EventDispatcher* _dispatcher;
	IEvent* _event;
	bool _hasDispatcher;
};

template <class Arg_t>
class Event : public IEvent
{
public:
	using Callback_f = Function<void(const Arg_t&)>;

	Event(Mirror::Type type, Callback_f callback)
	: _type(type)
	, _callback(callback)
	{
	}

	void Invoke(const Arg_t& arg)
	{
		_callback(arg);
	}

	virtual const Mirror::Type& GetEventType() const override
	{
		return _type;
	}

private:
	Mirror::Type _type;
	Callback_f const _callback;
};

template <class Class_t, class Arg_t>
Function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
{
	return std::bind(funcPointer, instance, std::placeholders::_1);
}

template <class Class_t, class Arg_t>
Function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Class_t, class Arg_t>
Function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Arg_t>
Function<void(const Arg_t&)> WrapFn(void(*funcPointer)(const Arg_t&))
{
	return std::function<void(const Arg_t&)>(funcPointer);
}

class EventDispatcher
{
	friend class EDReceipt;
public:
	using EventList = List<IEvent*>;
	using EventMap = UnorderedMap<Mirror::Type, EventList>;
	using Receipt = RefPtr<EDReceipt>;
	using ReceiptPtrList = List<EDReceipt*>;

	EventDispatcher();
	~EventDispatcher();

	template<class Arg_t, class Class_t = void>
	Receipt Register(void(*funcPointer)(const Arg_t&))
	{
		return Register<Arg_t>(WrapFn(funcPointer));
	}

	template <class Arg_t, class Class_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template <class Arg_t, class Class_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template <class Arg_t, class Class_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template<class Arg_t, class Class_t = void>
	Receipt Register(std::function<void(const Arg_t&)>& callback)
	{
		typedef Event<Arg_t> Event_t;

		Mirror::Type argType = Arg_t::MyType();
		IEvent* event = new Event_t(argType, callback);

		_events[argType].push_back(event);

		EDReceipt* r = new EDReceipt(const_cast<EventDispatcher*>(this), event);
		_receipts.push_back(r);
		_numRegisteredEvents++;

		assert(_numRegisteredEvents == _receipts.size());

		return r;
	}

	template <class Arg_t>
	void Dispatch(const Arg_t& arg)
	{
		assert(_numRegisteredEvents == _receipts.size());
		EventMap::iterator found = _events.find(Arg_t::MyType());
		if(found != _events.end())
		{
			for(IEvent* item : found->second)
			{
				assert(item);
				Event<Arg_t>* eventCasted = static_cast<Event<Arg_t>*>(item);
				if(eventCasted)
				{
					eventCasted->Invoke(arg);
				}
			}
		}
	}

	size_t GetNumRegisteredEvents();
	size_t GetNumRegisteredReceipts();

private:
	void Unregister(IEvent* event);

	EventMap       _events;
	int            _numRegisteredEvents;
	ReceiptPtrList _receipts;
};

CLOSE_NAMESPACE(Firestorm);
#endif