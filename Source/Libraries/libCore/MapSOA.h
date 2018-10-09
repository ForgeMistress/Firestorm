///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MapSOA
//
//  Defines an SOA implementation that maintains a primary key.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_MAPSOA_H_
#define LIBCORE_MAPSOA_H_
#pragma once

#include "SOA.h"

OPEN_NAMESPACE(Firestorm);

template<class PrimaryKey, class... TypesT>
struct MapSOA final
{
	using SOAType = SOA<TypesT...>;
	using KeyToIndexMap = UnorderedMap<PrimaryKey, size_t>;
	using IndexToKeyMap = Vector<PrimaryKey>;
public:
	MapSOA() {}
	~MapSOA() = default;

	void Insert(const PrimaryKey& key)
	{
		FIRE_ASSERT_MSG(Contains(key) == false, "can not double insert a key...");
		size_t index = _indexToKeyMap.size();
		_indexToKeyMap.push_back(key);
		_keyToIndex[index] = index;
	}

private:
	SOAType _soa;
	KeyToIndexMap _keyToIndexMap;
	IndexToKeyMap _indexToKeyMap;
};

CLOSE_NAMESPACE(Firestorm);

#endif