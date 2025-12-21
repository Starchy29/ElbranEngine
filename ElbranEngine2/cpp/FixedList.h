#pragma once
#include "Common.h"
#include "MemoryArena.h"

// a heap-allocated list with capacity known at initialization
template<class Type>
struct FixedList {
	Type* data;
	uint32_t size;

	FixedList() = default;

	void Initialize(Type* buffer) {
		data = buffer;
		size = 0u;
	}

	void Initialize(uint32_t capacity) {
		size = 0u;
		data = new Type[capacity] {};
	}

	void Initialize(uint32_t capacity, MemoryArena* arena) {
		size = 0u;
		data = (Type*)arena->Reserve(capacity * sizeof(Type));
	}

	void Release() { delete[] data; }
	Type& operator[](uint32_t index) { return data[index]; }
	void Clear() { size = 0; }

	void Add(Type element) {
		data[size] = element;
		size++;
	}

	Type* ReserveNext() {
		size++;
		return &data[size-1];
	}

	void QuickRemove(uint32_t index) {
		ASSERT(index < size);
		data[index] = data[size - 1];
		size--;
	}

	void SlowRemove(uint32_t index) {
		ASSERT(index < size);
		size--;
		for(uint32_t i = index; i < size; i++) {
			data[i] = data[i+1];
		}
	}
	
	int32_t IndexOf(Type element) {
		for(uint32_t i = 0; i < size; i++) {
			if(data[i] == element) return i;
		}
		return -1;
	}
};