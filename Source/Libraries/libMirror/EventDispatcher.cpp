///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EventDispatcher
//
//  Dispatches an event to its listeners.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EventDispatcher.h"

OPEN_NAMESPACE(Elf);

EDReceipt::EDReceipt(EventDispatcher* dispatcher, IEvent* event)
: _dispatcher(dispatcher)
, _event(event)
, _hasDispatcher(true)
{
}

EDReceipt::~EDReceipt()
{
	if (_hasDispatcher)
		_dispatcher->Unregister(_event);
}


void EDReceipt::DispatcherDeleted()
{
	_hasDispatcher = false;
}

EventDispatcher::EventDispatcher()
	: _numRegisteredEvents(0)
{
}
EventDispatcher::~EventDispatcher()
{
	for (auto r : _receipts)
		r->DispatcherDeleted();
	for (auto evtNames : _events)
	{
		for (auto event : evtNames.second)
		{
			delete event;
		}
	}
	_receipts.clear();
	_events.clear();
}

size_t EventDispatcher::GetNumRegisteredEvents()
{
	return _numRegisteredEvents;
}

size_t EventDispatcher::GetNumRegisteredReceipts()
{
	return _receipts.size();
}

void EventDispatcher::Unregister(IEvent* event)
{
	if (event)
	{
		auto found = _events.find(event->GetName());
		if (found != _events.end())
		{
			if (std::find(found->second.begin(), found->second.end(), event) != found->second.end())
			{
				found->second.remove(event);
				_numRegisteredEvents--;
			}
		}
		_receipts.remove_if([&event](const EDReceipt* r){
			return r->_event == event;
		});
	}
	assert(_numRegisteredEvents == _receipts.size());
}

CLOSE_NAMESPACE(Elf);