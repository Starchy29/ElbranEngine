#pragma once
#include <cassert>

// a locally-allocated unordered list with capacity known at compile time
template<class Type, unsigned int capacity>
class FixedList {
public:
	FixedList() {
		size = 0;
	}

	Type& operator[](int index) { return data[index]; }
	void Clear() { size = 0; }
	const Type* GetArray() const { return data; }
	unsigned int GetSize() const { return size; }

	void Add(Type element) {
		assert(size < capacity && "attempted to add to a full FixedList");
		data[size] = element;
		size++;
	}

	void RemoveAt(unsigned int index) {
		assert(index < size && "attempted to remove from an index out of range");
		data[index] = data[size - 1];
		size--;
	}

	int IndexOf(Type element) {
		for(int i = 0; i < size; i++) {
			if(data[i] == element) return i;
		}
		return -1;
	}

private:
	Type data[capacity];
	unsigned int size;
};

// a heap-allocated unordered list with capacity known at initialization
template<class Type>
class DynamicFixedList {
public:
	DynamicFixedList() {}

	void Allocate(unsigned int capacity) {
		size = 0;
		this->capacity = capacity;
		data = new Type[capacity] {};
	}

	void Release() { delete[] data; }
	Type& operator[](int index) { return data[index]; }
	unsigned int GetSize() const { return size; }
	void Clear() { size = 0; }
	const Type* GetArray() const { return data; }

	void Add(Type element) {
		assert(size < capacity && "attempted to add to a full FixedList");
		data[size] = element;
		size++;
	}

	void RemoveAt(unsigned int index) {
		assert(index < size && "attempted to remove from an index out of range");
		data[index] = data[size - 1];
		size--;
	}
	
	int IndexOf(Type element) {
		for(int i = 0; i < size; i++) {
			if(data[i] == element) return i;
		}
		return -1;
	}

private:
	Type* data;
	unsigned int capacity;
	unsigned int size;
};