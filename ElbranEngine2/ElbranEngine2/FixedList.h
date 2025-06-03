#pragma once
#include <cassert>
#include <iterator>
#include <type_traits>

// an array-based unordered list that is initialized with a set capacity which never changes
template<class Type>
class FixedList {
public:
	FixedList() {
		dataArray = nullptr;
		size = 0;
		capacity = 0;
	}

	FixedList(unsigned int capacity) {
		size = 0;
		this->capacity = capacity;
		dataArray = new Type[capacity] {};
	}

	void Release() {
		// will not call delete on elements because it is unknown if Type is a pointer
		delete[] dataArray; 
	}

	void ReleaseElements() {
		for(unsigned int i = 0; i < size; i++) {
			delete dataArray[i];
		}
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

	const Type* GetArray() const { return dataArray; }

private:
	Type* dataArray;
	unsigned int capacity;
	unsigned int size;
};