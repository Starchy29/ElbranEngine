#pragma once
#include <cassert>
#include <iterator>
#include <type_traits>

// an array-based unordered list that is initialized with a set capacity which never changes
template<class Type>
class FixedList
{
public:
	Type* dataArray;

	FixedList() {}

	FixedList(unsigned int capacity) {
		size = 0;
		this->capacity = capacity;
		dataArray = new Type[capacity] {};
	}

	Type& operator[](int index) {
		return dataArray[index];
	}

	unsigned int GetSize() const {
		return size;
	}

	void Add(Type element) {
		assert(size < capacity && "attempted to add to a full FixedList");
		dataArray[size] = element;
		size++;
	}

	void RemoveAt(unsigned int index) {
		assert(index < size && "attempted to remove from an index out of range");
		dataArray[index] = dataArray[size - 1];
		size--;
	}

private:
	unsigned int capacity;
	unsigned int size;
};