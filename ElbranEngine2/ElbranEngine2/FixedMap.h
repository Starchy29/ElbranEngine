#pragma once
#include <cassert>

// a hashmap with a set capacity known at initialization
template<class KeyType, class ValueType>
class FixedMap {
public:
	FixedMap() {
		capacity = 0;
		size = 0;
		hasher = nullptr;
		values = nullptr;
		fullSlots = nullptr;
	}

	FixedMap(unsigned int capacity, unsigned int (*hashFunction)(KeyType) = nullptr) :
		hasher{hashFunction},
		capacity{capacity},
		size{0}
	{
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
		unsigned int hashIndex = Hash(key);
		unsigned int index = hashIndex;
		for(unsigned int i = 0; i < capacity; i++) {
			index = (hashIndex + i) % capacity;
			if(keys[index] == key) {
				// key exists, update value
				values[index] = value;
				return;
			}
			if(fullSlots && !fullSlots[index]) {
				break;
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

		if(size == capacity) {
			delete[] fullSlots;
			fullSlots = nullptr;
		}
	}

	ValueType Get(const KeyType& key) const {
		unsigned int startIndex = Hash(key);
		for(unsigned int i = 0; i < capacity; i++) {
			unsigned int checkIndex = (startIndex + i) % capacity;
			if(key == keys[checkIndex]) {
				return values[checkIndex];
			}
		}

		assert(false && "Attempted to get a value from a FixedMap from a key not yet added");
		return {};
	}

private:
	unsigned int capacity;
	unsigned int size;
	unsigned int (*hasher)(KeyType);
	KeyType* keys;
	ValueType* values;
	bool* fullSlots;

	unsigned int Hash(const KeyType& key) const {
		if(hasher) {
			return hasher(key) % capacity;
		}

		// when there is no hash function, add to the first open slot
		return size;
	}
};