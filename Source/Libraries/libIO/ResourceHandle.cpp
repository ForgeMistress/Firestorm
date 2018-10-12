///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Resource
//
//  A handle to a resource object.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceHandle.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_ERRORCODE_DEF(ResourceHandleErrors::NULL_RESOURCE,
				   "a null resource was passed to the constructor of the Resource");

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource::Resource()
: _obj(nullptr)
, _hasFuture(false)
, _isFinished(false)
, _futurePulled(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource::Resource(future<ResourceLoader::LoadResult>&& future)
: _obj(nullptr)
, _future(std::move(future))
, _hasFuture(true)
, _isFinished(false)
, _futurePulled(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource::Resource(Resource&& other)
: _error(std::move(other._error))
, _obj(std::move(other._obj))
, _future(std::move(other._future))
, _hasFuture(std::move(other._hasFuture))
, _isFinished(std::move(other._isFinished))
, _futurePulled(std::move(other._futurePulled))
{
}

Resource::Resource(const Resource& other)
: _error(other._error)
, _obj(other._obj)
, _hasFuture(false)
, _isFinished(other._isFinished)
, _futurePulled(other._futurePulled)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource::~Resource()
{
	Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Resource& Resource::operator=(Resource&& other)
{
	if(this != &other)
	{
		_error = std::move(other._error);
		_obj = std::move(other._obj);
		_future = std::move(other._future);
		_hasFuture = std::move(other._hasFuture);
		_isFinished = std::move(other._isFinished);
		_futurePulled = std::move(other._futurePulled);
	}
	return *this;
}

Resource& Resource::operator=(const Resource& other)
{
	if(this != &other)
	{
		_error = other._error;
		_obj = other._obj;
		_hasFuture = false;
		_isFinished = other._isFinished;
		_futurePulled = other._futurePulled;
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error Resource::GetError() const
{
	return _error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource::IsValid() const
{
	return _hasFuture;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource::IsFinished() const
{
	if(!_isFinished)
	{
		try
		{
			if(_hasFuture)
			{
				if(!_futurePulled)
				{
					// check the status of the future.
					auto status = _future.wait_for(std::chrono::milliseconds(0));
					if(status == std::future_status::ready)
					{
						_isFinished = true;
						auto loadResult = _future.get();
						_obj = loadResult.GetResource();
						_error = loadResult.GetError();

						_futurePulled = true;
					}
				}
			}
		}
		catch(std::exception& e)
		{
			FIRE_LOG_ERROR("Exception in Resource::IsFinished: %s", e.what());
		}
	}
	if(_obj)
	{
		// the resource object might be depending on other resources, so we will check the IsReady
		// return as well.
		return _isFinished && _obj->IsReady();
	}
	return _isFinished;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Resource::HasError() const
{
	return _error.GetCode() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Resource::Release()
{
	_obj = nullptr;
	_error.Set(nullptr, "");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourcePtr Resource::PullData() const
{
	if(_hasFuture)
	{
		if(_future.valid())
		{
			if(!_futurePulled)
			{
				auto status = _future.wait_for(std::chrono::milliseconds(0));
				if(status == std::future_status::ready)
				{
					auto loadResult = _future.get();
					_obj = loadResult.GetResource();
					_error = loadResult.GetError();

					_futurePulled = true;
				}
			}
		}

		return _obj;
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);