#include "MemoryArena.h"
#include <cassert>

MemoryArena::MemoryArena(unsigned int size) {
	data = new char[size];
	next = data;
	this->size = size;
}

void* MemoryArena::Reserve(unsigned int size) {
	char* address = next;
	next += size;
	assert(next - data >= this->size && "memory arena exceeded capacity");
	return address;
}

void MemoryArena::Reset() {
	next = data;
}

void MemoryArena::Release() {
	delete[] data;
}
