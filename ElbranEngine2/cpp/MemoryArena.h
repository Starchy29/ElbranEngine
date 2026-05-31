#pragma once
#include <stdint.h>

class MemoryArena {
public:
	MemoryArena() = default;

	void Initialize(uint64_t size);
	void* Reserve(uint64_t bytes) const;
	MemoryArena ReserveSubArena(uint64_t bytes) const;
	void Clear();
	void Release();

private:
	uint8_t* data;
	mutable uint8_t* next;
	uint64_t size;
};

