#pragma once
#include <stdint.h>
#include "Common.h"

// a locally-allocated unordered list with capacity known at compile time
template<class Type, uint32_t capacity>
class FixedList {
private:
	Type data[capacity];
	uint32_t size;

public:
	FixedList() : size{0u} {}

	Type& operator[](uint32_t index) { return data[index]; }
	void Clear() { size = 0; }
	uint32_t Size() const { return size; }

	void Add(Type element) {
		ASSERT(size < capacity);
		data[size] = element;
		size++;
	}

	Type* ReserveNext() {
		ASSERT(size < capacity);
		size++;
		return &data[size-1];
	}

	void RemoveAt(uint16_t index) {
		ASSERT(index < size);
		data[index] = data[size - 1];
		size--;
	}

	int32_t IndexOf(Type element) {
		for(uint32_t i = 0; i < size; i++) {
			if(data[i] == element) return i;
		}
		return -1;
	}
};

// a heap-allocated unordered list with capacity known at initialization
template<class Type>
class DynamicFixedList {
private:
	Type* data;
	uint32_t capacity;
	uint32_t size;

public:
	DynamicFixedList() = default;

	void Allocate(uint32_t capacity) {
		size = 0u;
		this->capacity = capacity;
		data = new Type[capacity] {};
	}

	void Release() { delete[] data; }
	Type& operator[](uint32_t index) { return data[index]; }
	uint32_t Size() const { return size; }
	void Clear() { size = 0; }

	void Add(Type element) {
		ASSERT(size < capacity);
		data[size] = element;
		size++;
	}

	Type* ReserveNext() {
		ASSERT(size < capacity);
		size++;
		return &data[size-1];
	}

	void RemoveAt(uint32_t index) {
		ASSERT(index < size);
		data[index] = data[size - 1];
		size--;
	}
	
	int32_t IndexOf(Type element) {
		for(uint32_t i = 0; i < size; i++) {
			if(data[i] == element) return i;
		}
		return -1;
	}
};