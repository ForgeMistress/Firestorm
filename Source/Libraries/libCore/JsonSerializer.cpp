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

OPEN_NAMESPACE(Firestorm);

JsonSerializer::ObjectKeyList JsonSerializer::ObjectKeys()
{

}

void JsonSerializer::ObjectBegin(const Key& key)
{
}

void JsonSerializer::ObjectEnd(const Key& key)
{
}

size_t JsonSerializer::ArrayLength()
{
}

void JsonSerializer::ArrayBegin(const Key& key)
{
	_value[]
}

void JsonSerializer::ArrayEnd(const Key& key)  
{
}

void JsonSerializer::Write(const Key& key, int8_t& value)
{
}

void JsonSerializer::Write(const Key& key, int16_t& value)      
{
}

void JsonSerializer::Write(const Key& key, int32_t& value)      
{
}

void JsonSerializer::Write(const Key& key, int64_t& value)      
{
}

void JsonSerializer::Write(const Key& key, uint8_t& value)      
{
}

void JsonSerializer::Write(const Key& key, uint16_t& value)     
{
}

void JsonSerializer::Write(const Key& key, uint32_t& value)     
{
}

void JsonSerializer::Write(const Key& key, uint64_t& value)     
{
}

void JsonSerializer::Write(const Key& key, float& value)        
{
}

void JsonSerializer::Write(const Key& key, double& value)       
{
}

void JsonSerializer::Write(const Key& key, eastl::string& value)
{
}

void JsonSerializer::Read(const Key& key, uint32_t& value)
{
}

void JsonSerializer::Read(const Key& key, int8_t& value)       
{
}

void JsonSerializer::Read(const Key& key, int16_t& value)      
{
}

void JsonSerializer::Read(const Key& key, int32_t& value)      
{
}

void JsonSerializer::Read(const Key& key, int64_t& value)      
{
}

void JsonSerializer::Read(const Key& key, uint8_t& value)      
{
}

void JsonSerializer::Read(const Key& key, uint16_t& value)     
{
}

void JsonSerializer::Read(const Key& key, uint64_t& value)     
{
}

void JsonSerializer::Read(const Key& key, float& value)        
{
}

void JsonSerializer::Read(const Key& key, double& value)       
{
}

void JsonSerializer::Read(const Key& key, eastl::string& value)
{
}

bool JsonSerializer::IsWritingArray() const
{

}

bool JsonSerializer::IsWritingObject() const
{
}

CLOSE_NAMESPACE(Firestorm);