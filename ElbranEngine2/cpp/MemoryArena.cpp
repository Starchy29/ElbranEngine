#include "MemoryArena.h"
#include <string>
#include "Common.h"

void MemoryArena::Initialize(uint64_t size) {
	data = new uint8_t[size] {};
	next = data;
	this->size = size;
}

void* MemoryArena::Reserve(uint64_t bytes) {
	ASSERT(size - (next - data) >= bytes)
	ASSERT(*next == 0) // fails when a previous reservation exceeded its buffer
	uint8_t* address = next;
	next += bytes;
	return address;
}

MemoryArena MemoryArena::ReserveSubArena(uint64_t bytes) {
	MemoryArena result;
	result.data = (uint8_t*)Reserve(bytes);
	result.next = result.data;
	result.size = bytes;
	return result;
}

void MemoryArena::Clear() {
	next = data;
	memset(data, 0, size);
}

void MemoryArena::Release() {
	delete[] data;
}
