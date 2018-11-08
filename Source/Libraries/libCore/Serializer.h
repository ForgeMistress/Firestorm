///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Serializer
//
//  Base class of an object serializer.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_SERIALIZER_H_
#define LIBCORE_SERIALIZER_H_
#pragma once

OPEN_NAMESPACE(Firestorm);


/**
	Base class for a serializer type.
 **/
class Serializer
{
	// functions the end user should be calling
	template<class T>
	struct HasFuncInInterface
	{
		static constexpr bool value =
			eastl::is_same_v<T, int8_t>        ||
			eastl::is_same_v<T, int16_t>       ||
			eastl::is_same_v<T, int32_t>       ||
			eastl::is_same_v<T, int64_t>       ||
			eastl::is_same_v<T, uint8_t>       ||
			eastl::is_same_v<T, uint16_t>      ||
			eastl::is_same_v<T, uint32_t>      ||
			eastl::is_same_v<T, uint64_t>      ||
			eastl::is_same_v<T, float>         ||
			eastl::is_same_v<T, double>        ||
			eastl::is_same_v<T, eastl::string>;
	};

	// array, vector, list, etc...
	template<class T> struct IsSequentialContainer                          { static constexpr bool value = false; };
	template<class T> struct IsSequentialContainer<eastl::vector<T>>        { static constexpr bool value = true; };
	template<class T> struct IsSequentialContainer<eastl::list<T>>          { static constexpr bool value = true; };
	template<class T> struct IsSequentialContainer<eastl::unordered_set<T>> { static constexpr bool value = true; };

	// pretty much just unordered_map 
	template<class T>          struct IsKVContainer { static constexpr bool value = false; };
	template<class K, class V> struct IsKVContainer<eastl::unordered_map<K, V>> { static constexpr bool value = true; };
public:
	using Key = eastl::variant<size_t, eastl::string>;

private:
	template<class T>
	void SerializeSimple(const Key& key, T& value)
	{
		if(IsWriter())
			Write(key, value);
		else
			Read(key, value);
	}

	template<class T>
	void SerializeSequentialContainer(const Key& key, T& value)
	{
		ArrayBegin(key);
		size_t index = 0;
		for(auto&& val : value)
		{
			Serialize(index, val);
		}
		ArrayEnd(key);
	}

	template<class T>
	void SerializeArray(const Key& key, T& value)
	{
	}

	template<class T>
	void SerializeKVContainer(const Key& key, T& value)
	{
		ObjectBegin
	}

	template<class T>
	void SerializeObject(const Key& key, T& value)
	{

	}

public:
	enum {
		kRead = 1<<0,
		kWrite = 1<<1,
		kReadWrite = kRead | kWrite
	};

	Serializer(uint8_t flags = kReadWrite)
	: _flags(flags)
	{}

	template<class ValueT>
	void Serialize(const Key& key, ValueT& value)
	{
		// if we have the function right here in the Serializer, do a simple.
		if constexpr(HasFuncInInterface<ValueT>::value)
		{
			SerializeSimple(key, value);
		}
		else
		{
			if constexpr(IsSequentialContainer<ValueT>::value)
			{
				SerializeSequentialContainer(key, value);
			}
			else if constexpr(IsKVContainer<ValueT>::value)
			{
				SerializeKVContainer(key, value);
			}
			else
			{
				SerializeObject(key, value);
			}
		}
	}

	template<class ValueT>
	void ReadOnly(const Key& key, ValueT& value)
	{
		if(IsReader())
			Read(key, value);
	}

	template<class ValueT>
	void WriteOnly(const Key& key, ValueT& value)
	{
		if(IsWriter())
			Write(key, value);
	}

	bool IsReader() const { return _flags & kRead; }
	bool IsWriter() const { return _flags & kWrite; }

	// interface functions
protected:
	virtual void ObjectBegin(const Key& key) = 0;
	virtual void ObjectEnd(const Key& key) = 0;
	virtual void ArrayBegin(const Key& key) = 0;
	virtual void ArrayEnd(const Key& key) = 0;

	virtual void Write(const Key& key, int8_t& value) = 0;
	virtual void Write(const Key& key, int16_t& value) = 0;
	virtual void Write(const Key& key, int32_t& value) = 0;
	virtual void Write(const Key& key, int64_t& value) = 0;
	virtual void Write(const Key& key, uint8_t& value) = 0;
	virtual void Write(const Key& key, uint16_t& value) = 0;
	virtual void Write(const Key& key, uint32_t& value) = 0;
	virtual void Write(const Key& key, uint64_t& value) = 0;
	virtual void Write(const Key& key, float& value) = 0;
	virtual void Write(const Key& key, double& value) = 0;
	virtual void Write(const Key& key, eastl::string& value) = 0;

	virtual void Read(const Key& key, uint32_t& value) = 0;
	virtual void Read(const Key& key, int8_t& value) = 0;
	virtual void Read(const Key& key, int16_t& value) = 0;
	virtual void Read(const Key& key, int32_t& value) = 0;
	virtual void Read(const Key& key, int64_t& value) = 0;
	virtual void Read(const Key& key, uint8_t& value) = 0;
	virtual void Read(const Key& key, uint16_t& value) = 0;
	virtual void Read(const Key& key, uint64_t& value) = 0;
	virtual void Read(const Key& key, float& value) = 0;
	virtual void Read(const Key& key, double& value) = 0;
	virtual void Write(const Key& key, eastl::string& value) = 0;

private:
	uint8_t _flags{ kReadWrite };
};


CLOSE_NAMESPACE(Firestorm);

#endif