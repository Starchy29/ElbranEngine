#include "MemoryArena.h"
#include <string>
#include "Common.h"

void MemoryArena::Allocate(size_t size) {
	data = new uint8_t[size] {};
	next = data;
	this->size = size;
}

void* MemoryArena::Reserve(size_t bytes) {
	uint8_t* address = next;
	next += bytes;
	ASSERT(next - data < size);
	return address;
}

MemoryArena MemoryArena::ReserveSubArena(size_t bytes) {
	MemoryArena result;
	result.data = next;
	result.next = next;
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
