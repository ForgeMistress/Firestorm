///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JsonSerializer
//
//  Implementation of a serializer that formats json.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "JsonSerializer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

void JsonSerializer::Push(const Key& key)
{
	Json::Value& current = (*_valueStack.top());
	if(eastl::holds_alternative<eastl::string>(key))
	{
		FIRE_ASSERT(IsWritingObject());
		_valueStack.push(&current[eastl::get<eastl::string>(key).c_str()]);
	}
	else
	{
		FIRE_ASSERT(IsWritingArray());
		_valueStack.push(&current[(int)eastl::get<size_t>(key)]);
	}
}

void JsonSerializer::Pop()
{
	_valueStack.pop();
}

void JsonSerializer::Write(const Key& key, Json::Value& value)
{
	if(eastl::holds_alternative<eastl::string>(key))
	{
		(*_valueStack.top())[eastl::get<eastl::string>(key).c_str()] = value;
	}
	else
	{
		(*_valueStack.top())[(int)eastl::get<size_t>(key)] = value;
	}
}

JsonSerializer::ObjectKeyList JsonSerializer::ObjectKeys()
{
	if()
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::ObjectBegin(const Key& key)
{
	Write(key, Json::Value(Json::objectValue));
	Push(key);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::ObjectEnd(const Key& key)
{
	FIRE_ASSERT(IsWritingObject());
	Pop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t JsonSerializer::ArrayLength()
{
	FIRE_ASSERT(IsWritingArray());
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::ArrayBegin(const Key& key)
{
	Write(key, Json::Value(Json::arrayValue));
	Push(key);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::ArrayEnd(const Key& key)  
{
	FIRE_ASSERT(IsWritingArray());
	Pop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::Write(const Key& key, int8_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, int16_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, int32_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, int64_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, uint8_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, uint16_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, uint32_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, uint64_t& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, float& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, double& value)
{
	Write(key, Json::Value(value));
}

void JsonSerializer::Write(const Key& key, eastl::string& value)
{
	Write(key, Json::Value(value.c_str()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(impl);

template<class T>
void Read(Json::Value& v, const JsonSerializer::Key& key, T& outValue)
{
	if(eastl::holds_alternative<eastl::string>(key))
	{
		if constexpr(eastl::is_same_v<T, uint8_t> ||
		             eastl::is_same_v<T, uint16_t> ||
		             eastl::is_same_v<T, uint32_t>)
		{
			outValue = v[eastl::get<eastl::string>(key).c_str()].asUInt();
			return;
		}
		else if(eastl::is_same_v<T, int8_t> ||
		        eastl::is_same_v<T, int16_t> ||
		        eastl::is_same_v<T, int32_t>)
		{
			outValue = v[eastl::get<eastl::string>(key).c_str()].asInt();
			return;
		}
		else if(eastl::is_same_v<T, uint64_t>)
		{
			outValue = v[eastl::get<eastl::string>(key).c_str()].asUInt64();
			return;
		}
		else if(eastl::is_same_v<T, int64_t>)
		{
			outValue = v[eastl::get<eastl::string>(key).c_str()].asInt64();
			return;
		}
		else if(eastl::is_same_v<T, eastl::string>)
		{
			outValue = v[eastl::get<eastl::string>(key).c_str()].asCString();
			return;
		}
	}
	else
	{
		if constexpr(eastl::is_same_v<T, uint8_t> ||
		             eastl::is_same_v<T, uint16_t> ||
		             eastl::is_same_v<T, uint32_t>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asUInt();
			return;
		}
		else if(eastl::is_same_v<T, uint64_t>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asUInt64();
			return;
		}
		else if(eastl::is_same_v<T, int8_t> ||
		        eastl::is_same_v<T, int16_t> ||
		        eastl::is_same_v<T, int32_t>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asInt();
			return;
		}
		else if(eastl::is_same_v<T, int64_t>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asInt64();
			return;
		}
		else if(eastl::is_same_v<T, float>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asFloat();
			return;
		}
		else if(eastl::is_same_v<T, double>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asDouble();
			return;
		}
		else if(eastl::is_same_v<T, eastl::string>)
		{
			outValue = v[(int)eastl::get<size_t>(key)].asCString();
			return;
		}
	}
}
CLOSE_NAMESPACE(impl);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void JsonSerializer::Read(const Key& key, int8_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, int16_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, int32_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, int64_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, uint8_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, uint16_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, uint32_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, uint64_t& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, float& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, double& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

void JsonSerializer::Read(const Key& key, eastl::string& value)
{
	impl::Read((*_valueStack.top()), key, value);
}

bool JsonSerializer::IsWritingArray() const
{
	FIRE_ASSERT(!this->_valueStack.empty());
	return _valueStack.top()->type() == Json::arrayValue;
}

bool JsonSerializer::IsWritingObject() const
{
	FIRE_ASSERT(!this->_valueStack.empty());
	return _valueStack.top()->type() == Json::objectValue;
}

CLOSE_NAMESPACE(Firestorm);