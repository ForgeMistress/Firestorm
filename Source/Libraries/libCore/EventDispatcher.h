///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EventDispatcher
//
//  Dispatches events to its listeners.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_EVENTDISPATCHER_H_
#define LIBMIRROR_EVENTDISPATCHER_H_
#pragma once

#include <libCore/RefPtr.h>
#include "Object.h"
#include <typeinfo>

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(Mirror);

class Object;

CLOSE_NAMESPACE(Mirror);

class IEvent
{
public:
	virtual FireClassID GetEventType() const = 0;
};

class EventDispatcher;

template <class Arg_t>
class Event : public IEvent
{
public:
	using Callback_f = function<void(const Arg_t&)>;

	Event(FireClassID type, Callback_f callback)
	: _type(type)
	, _callback(callback)
	{
	}

	void Invoke(const Arg_t& arg)
	{
		_callback(arg);
	}

	virtual FireClassID GetEventType() const override
	{
		return _type;
	}

private:
	FireClassID _type;
	Callback_f const _callback;
};

class EventDispatcher
{
	friend class EDReceipt;
private:
	class EDReceipt final
	{
		friend class EventDispatcher;
	public:
		EDReceipt(EventDispatcher* dispatcher, IEvent* event);
		~EDReceipt();

		void DispatcherDeleted();

	private:
		EventDispatcher* _dispatcher;
		IEvent* _event;
		bool _hasDispatcher;
	};

public:
	using EventList = list<IEvent*>;
	using EventMap = unordered_map<FireClassID, EventList>;
	using Receipt = RefPtr<EDReceipt>;
	using ReceiptPtrList = list<WeakPtr<EDReceipt>>;

	EventDispatcher();
	~EventDispatcher();

	template<class Arg_t, class Function>
	Receipt Register(Function& callback)
	{
		typedef Event<Arg_t> Event_t;

		IEvent* event = new Event_t(Arg_t::MyType(), callback);

		_mutex.lock();
		_events[Arg_t::MyType()].push_back(event);

		Receipt r = make_shared<EDReceipt>(const_cast<EventDispatcher*>(this), event);
		_receipts.push_back(r);
		_numRegisteredEvents++;
		_mutex.unlock();

		FIRE_ASSERT(_numRegisteredEvents == _receipts.size());

		return r;
	}

	template <class Arg_t>
	void Dispatch(const Arg_t& arg)
	{
		FIRE_ASSERT(_numRegisteredEvents == _receipts.size());
		std::scoped_lock lock(_mutex);
		EventMap::iterator found = _events.find(Arg_t::MyType());
		if(found != _events.end())
		{
			for(IEvent* item : found->second)
			{
				FIRE_ASSERT(item);
				Event<Arg_t>* eventCasted = static_cast<Event<Arg_t>*>(item);
				if(eventCasted)
				{
					eventCasted->Invoke(arg);
				}
			}
		}
	}

	template <class Arg_t>
	bool HasRegisteredEvents()
	{
		std::scoped_lock lock(_mutex);
		if(_events.find(Arg_t::MyType()) != _events.end())
			return !_events[Arg_t::MyType()].empty();
		return true;
	}

	size_t GetNumRegisteredEvents();
	size_t GetNumRegisteredReceipts();

private:
	void Unregister(IEvent* event);

	std::mutex     _mutex;
	EventMap       _events;
	int            _numRegisteredEvents;
	ReceiptPtrList _receipts;
};

CLOSE_NAMESPACE(Firestorm);
#endif