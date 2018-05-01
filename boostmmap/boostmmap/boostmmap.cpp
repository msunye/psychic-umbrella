// boostmmap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <boost/interprocess/managed_mapped_file.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/allocators/allocator.hpp>


#include <boost/unordered_map.hpp>     //boost::unordered_map


#include <functional>                  //std::equal_to
#include <boost/functional/hash.hpp>   //boost::hash

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace boost::interprocess;

	managed_mapped_file mfile(open_or_create
		, "c:\\users\\marc\\mmfile" //Mapped file name
		, 65536);        //Mapped file size

	//Note that unordered_map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
	//so the allocator must allocate that pair.
	typedef int    KeyType;
	typedef float  MappedType;
	typedef std::pair<const int, float> ValueType;

	//Typedef the allocator
	typedef allocator<ValueType, managed_mapped_file::segment_manager> MapAllocator;
	MapAllocator mapAllocator(mfile.get_segment_manager());

	//Alias an unordered_map of ints that uses the previous STL-like allocator.
	typedef boost::unordered_map
		< KeyType, MappedType
		, boost::hash<KeyType>, std::equal_to<KeyType>
		, MapAllocator>
		MyHashMap;

	//Construct a shared memory hash map.
	//Note that the first parameter is the initial bucket count and
	//after that, the hash function, the equality function and the allocator
	MyHashMap *myhashmap = mfile.find_or_construct<MyHashMap>("MyHashMap")  //object name
		(3, boost::hash<int>(), std::equal_to<int>()                  //
			, mapAllocator);                         //allocator instance

	std::cout << "n:" << myhashmap->size();

	for (int i = 0; i < 100; ++i) {
		myhashmap->operator[](i+100) = (float)i * 2;
	}

	//Insert data in the hash map
	for (int i = 0; i < 100; ++i) {
		myhashmap->insert(ValueType(i, (float)i));
	}
	return 0;
}

