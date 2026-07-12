#pragma once
#include "Common.h"
#include "MemoryArena.h"

// a list with capacity known at initialization
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

	void Initialize(uint32_t capacity, const MemoryArena* arena) {
		size = 0u;
		data = (Type*)arena->Reserve(capacity * sizeof(Type));
	}

	void Release() { delete[] data; }
	Type& operator[](uint32_t index) { return data[index]; }
	const Type& operator[](uint32_t index) const { return data[index]; }
	void Clear() { size = 0; }

	void Add(Type element) {
		data[size] = element;
		size++;
	}

	Type* ReserveNext() {
		size++;
		return &data[size-1];
	}

	void SwapRemove(uint32_t index) {
		ASSERT(index < size)
		data[index] = data[size - 1];
		size--;
	}

	void ShiftRemove(uint32_t index) {
		ASSERT(index < size)
		size--;
		for(uint32_t i = index; i < size; i++) data[i] = data[i+1];
	}
	
	int32_t IndexOf(Type element) {
		for(uint32_t i = 0; i < size; i++) if(data[i] == element) return i;
		return -1;
	}
};

// a hashmap with a set capacity known at initialization
template<class KeyType, class ValueType>
class FixedMap {
public:
	FixedMap() = default;

	void Initialize(uint16_t capacity, uint32_t (*hashFunction)(KeyType) = nullptr) {
		size = 0;
		this->capacity = capacity;
		hasher = hashFunction;
		pairs = new KeyVal[capacity] {};
	}

	void Initialize(const MemoryArena* arena, uint16_t capacity, uint32_t (*hashFunction)(KeyType) = nullptr) {
		size = 0;
		this->capacity = capacity;
		hasher = hashFunction;
		pairs = (KeyVal*)arena->Reserve(capacity * sizeof(KeyVal));
	}

	void Release() {
		delete[] pairs;
	}

	void Set(const KeyType& key, ValueType value) {
		// check if the key already exists in the map
		uint32_t hashIndex = Hash(key);
		uint32_t index = hashIndex;
		for(uint32_t i = 0; i < capacity; i++) {
			index = (hashIndex + i) % capacity;
			if(!pairs[index].occupied) {
				break;
			}
			if(pairs[index].key == key) {
				// key exists, update value
				pairs[index].value = value;
				return;
			}
		}

		// add the new key/value pair
		if(size >= capacity) {
			ASSERT(false);
			return;
		}
		size++;

		// shift items up a slot to make an open space at the hash index
		pairs[index].occupied = true;
		if(index < hashIndex) {
			for(; index > 0; index--) {
				pairs[index] = pairs[index - 1];
			}

			// loop around to the end
			index = capacity - 1;
			pairs[0] = pairs[index];
		}

		for(; index > hashIndex; index--) {
			pairs[index] = pairs[index - 1];
		}

		pairs[hashIndex].key = key;
		pairs[hashIndex].value = value;
	}

	ValueType Get(const KeyType& key) const {
		uint32_t startIndex = Hash(key);
		for(uint32_t i = 0; i < capacity; i++) {
			uint32_t checkIndex = (startIndex + i) % capacity;
			if(key == pairs[checkIndex].key) return pairs[checkIndex].value;
		}

		ASSERT(false); // Attempted to get a value from a FixedMap from a key not yet added
		return {};
	}

private:
	struct KeyVal {
		KeyType key;
		ValueType value;
		bool occupied;
	};

	uint32_t capacity;
	uint32_t size;
	uint32_t (*hasher)(KeyType);
	KeyVal* pairs;

	uint32_t Hash(const KeyType& key) const {
		if(hasher) { return hasher(key) % capacity; }
		return size; // when there is no hash function, add to the first open slot
	}
};