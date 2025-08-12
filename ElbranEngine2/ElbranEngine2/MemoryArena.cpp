#include "MemoryArena.h"
#include <cassert>
#include <string>

void MemoryArena::Allocate(size_t size) {
	data = new char[size];
	next = data;
	this->size = size;
}

void* MemoryArena::Reserve(size_t bytes, bool zeroed) {
	char* address = next;
	next += bytes;
	assert(next - data < size && "memory arena exceeded capacity");
	if(zeroed) {
		memset(address, 0, bytes);
	}
	return address;
}

void MemoryArena::Reset() {
	next = data;
}

void MemoryArena::Release() {
	delete[] data;
}
