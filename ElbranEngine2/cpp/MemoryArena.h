#pragma once
#include <stdint.h>

class MemoryArena {
public:
	MemoryArena() = default;

	void Initialize(uint64_t size);
	void* Reserve(uint64_t bytes);
	MemoryArena ReserveSubArena(uint64_t bytes);
	void Clear();
	void Release();

private:
	uint8_t* data;
	uint8_t* next;
	uint64_t size;
};

