#pragma once
#include <stdint.h>

class MemoryArena {
public:
	MemoryArena() = default;

	void Allocate(size_t size);
	void* Reserve(size_t bytes);
	MemoryArena ReserveSubArena(size_t bytes);
	void Clear();
	void Release();

private:
	uint8_t* data;
	uint8_t* next;
	size_t size;
};

