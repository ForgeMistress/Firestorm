///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JsonSerializer
//
//  Implementation of a serializer that formats json.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_JSONSERIALIZER_H_
#define LIBCORE_JSONSERIALIZER_H_
#pragma once

#include <EASTL/stack.h>

#include <json/json.h>
#include "Serializer.h"

OPEN_NAMESPACE(Firestorm);

class JsonSerializer : public Serializer
{
public:
	ObjectKeyList ObjectKeys()       override;
	void ObjectBegin(const Key& key) override;
	void ObjectEnd(const Key& key)   override;

	size_t ArrayLength()            override;
	void ArrayBegin(const Key& key) override;
	void ArrayEnd(const Key& key)   override;

	void Write(const Key& key, int8_t& value)        override;
	void Write(const Key& key, int16_t& value)       override;
	void Write(const Key& key, int32_t& value)       override;
	void Write(const Key& key, int64_t& value)       override;
	void Write(const Key& key, uint8_t& value)       override;
	void Write(const Key& key, uint16_t& value)      override;
	void Write(const Key& key, uint32_t& value)      override;
	void Write(const Key& key, uint64_t& value)      override;
	void Write(const Key& key, float& value)         override;
	void Write(const Key& key, double& value)        override;
	void Write(const Key& key, eastl::string& value) override;

	void Read(const Key& key, int8_t& value)        override;
	void Read(const Key& key, int16_t& value)       override;
	void Read(const Key& key, int32_t& value)       override;
	void Read(const Key& key, int64_t& value)       override;
	void Read(const Key& key, uint8_t& value)       override;
	void Read(const Key& key, uint16_t& value)      override;
	void Read(const Key& key, uint32_t& value)      override;
	void Read(const Key& key, uint64_t& value)      override;
	void Read(const Key& key, float& value)         override;
	void Read(const Key& key, double& value)        override;
	void Read(const Key& key, eastl::string& value) override;
private:
	void Write(const Key& key, Json::Value& value);

	void Push(const Key& key);
	void Pop();

	bool IsWritingArray() const;
	bool IsWritingObject() const;

	Json::Value _value;
	eastl::stack<Json::Value*> _valueStack;
};

CLOSE_NAMESPACE(Firestorm);

#endif