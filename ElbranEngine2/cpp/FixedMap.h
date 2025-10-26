#pragma once
#include <stdint.h>
#include <cassert>

// a hashmap with a set capacity known at compile time
template<class KeyType, class ValueType, uint32_t capacity>
class FixedMap {
public:
	uint32_t (*hasher)(KeyType);

	FixedMap() : 
		size{0u},
		hasher{nullptr}
	{ }

	void Set(const KeyType& key, ValueType value) {
		// check if the key already exists in the map
		uint32_t hashIndex = Hash(key);
		uint32_t index = hashIndex;
		for(uint32_t i = 0; i < capacity; i++) {
			index = (hashIndex + i) % capacity;
			if(!fullSlots[index]) {
				break;
			}
			if(keys[index] == key) {
				// key exists, update value
				values[index] = value;
				return;
			}
		}

		// add the new key/value pair
		if(size >= capacity) {
			assert(false && "Attempted to add to a full FixedMap");
			return;
		}
		size++;

		// shift items up a slot to make an open space at the hash index
		fullSlots[index] = true;
		if(index < hashIndex) {
			for(; index > 0; index--) {
				keys[index] = keys[index - 1];
				values[index] = values[index - 1];
			}

			// loop around to the end
			index = capacity - 1;
			keys[0] = keys[index];
			values[0] = values[index];
		}

		for(; index > hashIndex; index--) {
			keys[index] = keys[index - 1];
			values[index] = values[index - 1];
		}

		keys[hashIndex] = key;
		values[hashIndex] = value;
	}

	ValueType Get(const KeyType& key) const {
		uint32_t startIndex = Hash(key);
		for(uint32_t i = 0; i < capacity; i++) {
			uint32_t checkIndex = (startIndex + i) % capacity;
			if(key == keys[checkIndex]) return values[checkIndex];
		}

		assert(false && "Attempted to get a value from a FixedMap from a key not yet added");
		return {};
	}

private:
	uint32_t size;
	KeyType keys[capacity];
	ValueType values[capacity];
	bool fullSlots[capacity];

	uint32_t Hash(const KeyType& key) const {
		if(hasher) { return hasher(key) % capacity; }
		return size; // when there is no hash function, add to the first open slot
	}
};

// a hashmap with a set capacity known at initialization
template<class KeyType, class ValueType>
class DynamicFixedMap {
public:
	DynamicFixedMap() {}

	void Initialize(uint16_t capacity, uint32_t (*hashFunction)(KeyType) = nullptr) {
		size = 0;
		this->capacity = capacity;
		hasher = hashFunction;
		keys = new KeyType[capacity] {};
		values = new ValueType[capacity] {};
		fullSlots = new bool[capacity] {};
	}

	void Release() {
		delete[] keys;
		delete[] values;
		delete[] fullSlots;
	}

	void Set(const KeyType& key, ValueType value) {
		// check if the key already exists in the map
		uint32_t hashIndex = Hash(key);
		uint32_t index = hashIndex;
		for(uint32_t i = 0; i < capacity; i++) {
			index = (hashIndex + i) % capacity;
			if(!fullSlots[index]) {
				break;
			}
			if(keys[index] == key) {
				// key exists, update value
				values[index] = value;
				return;
			}
		}

		// add the new key/value pair
		if(size >= capacity) {
			assert(false && "Attempted to add to a full FixedMap");
			return;
		}
		size++;

		// shift items up a slot to make an open space at the hash index
		fullSlots[index] = true;
		if(index < hashIndex) {
			for(; index > 0; index--) {
				keys[index] = keys[index - 1];
				values[index] = values[index - 1];
			}

			// loop around to the end
			index = capacity - 1;
			keys[0] = keys[index];
			values[0] = values[index];
		}

		for(; index > hashIndex; index--) {
			keys[index] = keys[index - 1];
			values[index] = values[index - 1];
		}

		keys[hashIndex] = key;
		values[hashIndex] = value;
	}

	ValueType Get(const KeyType& key) const {
		uint32_t startIndex = Hash(key);
		for(uint32_t i = 0; i < capacity; i++) {
			uint32_t checkIndex = (startIndex + i) % capacity;
			if(key == keys[checkIndex]) return values[checkIndex];
		}

		assert(false && "Attempted to get a value from a FixedMap from a key not yet added");
		return {};
	}

private:
	uint32_t capacity;
	uint32_t size;
	uint32_t (*hasher)(KeyType);
	KeyType* keys;
	ValueType* values;
	bool* fullSlots;

	uint32_t Hash(const KeyType& key) const {
		if(hasher) { return hasher(key) % capacity; }
		return size; // when there is no hash function, add to the first open slot
	}
};