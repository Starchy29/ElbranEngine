#pragma once
#include <stdint.h>

class MemoryArena
{
public:
	MemoryArena() {}

	void Allocate(size_t size);
	void* Reserve(size_t bytes, bool zeroed = false);
	void Reset();
	void Release();

	void operator=(const MemoryArena&) = delete;
	MemoryArena(const MemoryArena&) = delete;

private:
	char* data;
	char* next;
	size_t size;
};

