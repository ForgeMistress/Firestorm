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

#include <EASTL/array.h>

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(traits);

template<class T>
struct IsSequentialContainer
{
	static constexpr bool value = false;
};

template<class T>
struct IsSequentialContainer<eastl::vector<T>>
{
	static constexpr bool value = true;
};

template<class T, size_t N>
struct IsSequentialContainer<eastl::array<T, N>>
{
	static constexpr bool value = true;
};

template<class T>
struct IsIterableContainer
{
	static constexpr bool value = false;
};

template<class T>
struct IsIterableContainer<eastl::list<T>>
{
	static constexpr bool value = true;
};

template<class T>
struct IsIterableContainer<eastl::unordered_set<T>>
{
	static constexpr bool value = true;
};

template<class T>
struct IsKVContainer
{
	static constexpr bool value = false;
};

template<class K, class V>
struct IsKVContainer<eastl::unordered_map<K,V>>
{
	static constexpr bool value = true;
};

CLOSE_NAMESPACE(traits);

class Serializer;

OPEN_NAMESPACE(detail);

template<typename T>
struct HasSerializeMethod
{
	template<typename U, void(U::*)(Serializer&)>
	struct SFINAE
	{
	};
	template<typename U> static char Test(SFINAE<U, &U::Serialize>*);
	template<typename U> static int Test(...);
	static constexpr bool value = sizeof(Test<T>(0)) == sizeof(char);
};

template<class T>
struct IsSimple
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
template<class T>
struct IsSequentialContainer
{ 
	static constexpr bool value = traits::IsSequentialContainer<T>::value;
};

template<class T>
struct IsIterableContainer
{
	static constexpr bool value = traits::IsIterableContainer<T>::value;
};

// pretty much just unordered_map 
template<class T>
struct IsKVContainer
{
	static constexpr bool value = traits::IsKVContainer<T>::value;
};

CLOSE_NAMESPACE(detail);

/**
	Base class for a serializer type.
 **/
class Serializer
{
public:
	enum {
		kRead = 1<<0,
		kWrite = 1<<1,
		kReadWrite = kRead | kWrite
	};

	using Key = eastl::variant<size_t, eastl::string>;
	using ObjectKeyList = eastl::vector<eastl::string>;

	Serializer(uint8_t flags = kReadWrite)
	: _flags(flags)
	{
	}

	bool IsReader() const
	{ 
		return _flags & kRead;
	}

	bool IsWriter() const
	{
		return _flags & kWrite;
	}

	template<class ValueT>
	void Serialize(const Key& key, ValueT& value)
	{
		// if we have the function right here in the Serializer, do a simple.
		if constexpr(detail::IsSimple<ValueT>::value)
		{
			SerializeSimple(key, value);
			return;
		}
		else
		{
			if constexpr(detail::IsSequentialContainer<ValueT>::value)
			{
				SerializeSequentialContainer(key, value);
				return;
			}
			else if constexpr(detail::IsIterableContainer<ValueT>::value)
			{
				SerializeIterableContainer(key, value);
				return;
			}
			else if constexpr(detail::IsKVContainer<ValueT>::value)
			{
				SerializeKVContainer(key, value);
				return;
			}
			else if(eastl::is_class<ValueT>::value)
			{
				SerializeObject(key, value);
				return;
			}
		}
	}

	template<class ValueT>
	void ReadOnly(const Key& key, ValueT& value)
	{
		if(IsReader())
			Serialize(key, value);
	}

	template<class ValueT>
	void WriteOnly(const Key& key, ValueT& value)
	{
		if(IsWriter())
			Serialize(key, value);
	}

protected:
	/**
		This function should return a deterministic list of the keys for each field at the current level.
		This is used for Key/Value associative containers (unordered_map<K,V>) and is called
		immediately after ObjectBegin during Read operations.
	**/
	virtual ObjectKeyList ObjectKeys()       = 0;
	virtual void ObjectBegin(const Key& key) = 0;
	virtual void ObjectEnd(const Key& key)   = 0;

	/**
		This function should return the length of the "array" style structure. This is true for
		Sequential containers(vector, foo[], etc...) and Iterable containers (list<>, unordered_set<>, etc...).
	**/
	virtual size_t ArrayLength()            = 0;
	virtual void ArrayBegin(const Key& key) = 0;
	virtual void ArrayEnd(const Key& key)   = 0;

	virtual void Write(const Key& key, int8_t& value)        = 0;
	virtual void Write(const Key& key, int16_t& value)       = 0;
	virtual void Write(const Key& key, int32_t& value)       = 0;
	virtual void Write(const Key& key, int64_t& value)       = 0;
	virtual void Write(const Key& key, uint8_t& value)       = 0;
	virtual void Write(const Key& key, uint16_t& value)      = 0;
	virtual void Write(const Key& key, uint32_t& value)      = 0;
	virtual void Write(const Key& key, uint64_t& value)      = 0;
	virtual void Write(const Key& key, float& value)         = 0;
	virtual void Write(const Key& key, double& value)        = 0;
	virtual void Write(const Key& key, eastl::string& value) = 0;

	virtual void Read(const Key& key, uint32_t& value)      = 0;
	virtual void Read(const Key& key, int8_t& value)        = 0;
	virtual void Read(const Key& key, int16_t& value)       = 0;
	virtual void Read(const Key& key, int32_t& value)       = 0;
	virtual void Read(const Key& key, int64_t& value)       = 0;
	virtual void Read(const Key& key, uint8_t& value)       = 0;
	virtual void Read(const Key& key, uint16_t& value)      = 0;
	virtual void Read(const Key& key, uint64_t& value)      = 0;
	virtual void Read(const Key& key, float& value)         = 0;
	virtual void Read(const Key& key, double& value)        = 0;
	virtual void Read(const Key& key, eastl::string& value) = 0;

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
		if(IsWriter())
		{
			for(auto&& val : value)
			{
				Serialize(index, val);
				index++;
			}
		}
		else if(IsReader())
		{
			size_t arrayLength = ArrayLength();
			for(size_t i=0; i<arrayLength; ++i)
			{
				T::value_type v;
				Serialize(i, v);
				value.push_back(v);
			}
		}

		ArrayEnd(key);
	}

	template<class T>
	void SerializeArray(const Key& key, T& value)
	{
		ArrayBegin(key);

		size_t arraySize = ArraySize();

		for(size_t i=0; i<arraySize; ++i)
		{
			Serialize(i, value[i]);
		}

		ArrayEnd(key);
	}

	template<class T>
	void SerializeKVContainer(const Key& key, T& value)
	{
		ObjectBegin(key);

		auto&& keyList = ObjectKeys();
		for(auto&& key : keyList)
		{
			Serialize(key, value[key]);
		}

		ObjectEnd(key);
	}

	template<class T>
	void SerializeObject(const Key& key, T& value)
	{
		if constexpr(detail::HasSerializeMethod<T>::value)
		{
			ObjectBegin(key);
			value.Serialize(*this);
			ObjectEnd(key);
			return;
		}
		else
		{
			void Serialize(const Key&, T&);
			Serialize(key, value);
		}
	}

	uint8_t _flags{ kReadWrite };
};


CLOSE_NAMESPACE(Firestorm);

#endif