#include "MemoryArena.h"
#include <string>
#include "Common.h"

void MemoryArena::Allocate(size_t size) {
	data = new char[size];
	next = data;
	this->size = size;
}

void* MemoryArena::Reserve(size_t bytes, bool zeroed) {
	char* address = next;
	next += bytes;
	ASSERT(next - data < size);
	if(zeroed) {
		memset(address, 0, bytes);
	}
	return address;
}

void MemoryArena::Clear() {
	next = data;
}

void MemoryArena::Release() {
	delete[] data;
}
