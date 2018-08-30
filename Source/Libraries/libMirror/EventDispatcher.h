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

OPEN_NAMESPACE(Elf);

OPEN_NAMESPACE(Mirror);

class Object;

CLOSE_NAMESPACE(Mirror);

class IEvent
{
public:
	virtual const String& GetName() const = 0;
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
	using Callback_f = std::function<void(const Arg_t&)>;

	Event(const String& name, Callback_f callback)
	: _name(name)
	, _callback(callback)
	{
	}

	void Invoke(const Arg_t& arg)
	{
		_callback(arg);
	}

	virtual const String& GetName() const override { return _name; }

private:
	String _name;
	Callback_f const _callback;
};

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
{
	return std::bind(funcPointer, instance, std::placeholders::_1);
}

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Class_t, class Arg_t>
std::function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Arg_t>
std::function<void(const Arg_t&)> WrapFn(void(*funcPointer)(const Arg_t&))
{
	return std::function<void(const Arg_t&)>(funcPointer);
}

class EventDispatcher
{
	friend class EDReceipt;
public:
	typedef RefPtr<EDReceipt> Receipt;
	EventDispatcher();
	~EventDispatcher();

	template <class Class_t, class Arg_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template <class Class_t, class Arg_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template <class Class_t, class Arg_t>
	Receipt Register(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
	{
		return Register<Arg_t>(WrapFn(funcPointer, instance));
	}

	template<class Arg_t>
	Receipt Register(void(*funcPointer)(const Arg_t&))
	{
		return Register<Arg_t>(WrapFn(funcPointer));
	}

	template<class Arg_t>
	Receipt Register(std::function<void(const Arg_t&)>& callback)
	{
		typedef Event<Arg_t> Event_t;

		// Eventually I'd like to replace Type::EVENT_NAME with the rttr type to use that as
		// a string key, but this works for a proof of concept.
		IEvent* event = new Event_t(Arg_t::EVENT_NAME, callback);

		_events[Arg_t::EVENT_NAME].push_back(event);

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
		auto found = _events.find(Arg_t::EVENT_NAME);
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

	UnorderedMap<String, List<IEvent*>> _events;
	int                                 _numRegisteredEvents;
	List<EDReceipt*>                    _receipts;
};

CLOSE_NAMESPACE(Elf);
#endif