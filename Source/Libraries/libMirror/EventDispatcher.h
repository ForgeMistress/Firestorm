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

#pragma optimize("", off)
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
	//template<class T, class... Args> friend std::shared_ptr<T> std::make_shared<T, Args...>(Args&&...);
private:
	EDReceipt(EventDispatcher* dispatcher, IEvent* event);
	~EDReceipt();

	void DispatcherDeleted()
	{
		_hasDispatcher = false;
	}

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

class EventDispatcher
{
	friend class EDReceipt;
public:
	EventDispatcher()
	: _numRegisteredEvents(0)
	{
	}
	~EventDispatcher()
	{
		for(auto r : _receipts)
			r->DispatcherDeleted();
		for(auto evtNames : _events)
		{
			for(auto event : evtNames.second)
			{
				delete event;
			}
		}
		_receipts.clear();
		_events.clear();
	}

	template<class Arg_t>
	EDReceipt Register(std::function<void(const Arg_t&)> callback)
	{
		typedef Event<Arg_t> Event_t;
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

	int GetNumRegisteredEvents()
	{
		return _numRegisteredEvents;
	}

	int GetNumRegisteredReceipts()
	{
		return _receipts.size();
	}

private:
	void Unregister(IEvent* event)
	{
		if(event)
		{
			auto found = _events.find(event->GetName());
			if(found != _events.end())
			{
				if(std::find(found->second.begin(), found->second.end(), event) != found->second.end())
				{
					found->second.remove(event);
					_numRegisteredEvents--;
				}
			}
			_receipts.remove_if([&event](EDReceipt* r){
				return r->_event == event;
			});
		}
		assert(_numRegisteredEvents == _receipts.size());
	}

	UnorderedMap<String, List<IEvent*>> _events;
	int _numRegisteredEvents;
	List<EDReceipt*> _receipts;
	UnorderedSet<String> _eventTypes;
};

EDReceipt::~EDReceipt()
{
	if(_hasDispatcher)
		_dispatcher->Unregister(_event);
}

CLOSE_NAMESPACE(Elf);
#endif