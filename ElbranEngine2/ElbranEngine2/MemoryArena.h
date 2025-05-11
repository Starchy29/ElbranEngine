#pragma once
class MemoryArena
{
public:
	MemoryArena(unsigned int size);

	void* Reserve(unsigned int size);
	void Reset();
	void Release();

	void operator=(const MemoryArena&) = delete;
	MemoryArena(const MemoryArena&) = delete;

private:
	char* data;
	char* next;
	unsigned int size;
};

