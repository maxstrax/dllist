#ifndef MEMORY_LEAK_H
#define MEMORY_LEAK_H

#include "../include/cpp/mmalloc.hpp"
#include <new>

template<class T, size_t N>
class MemoryManagerTN : MemoryManagerRAII {
	T memoryPool[N];
	size_t allocatedCount;
public:
	MemoryManagerTN()
		: MemoryManagerRAII((mallocFunctor)[this](size_t size) -> void* {
		if (!size)
			throw std::exception("Wrong memory size 0 requested.");
		if (size % sizeof(T) != 0)
			throw std::exception("Wrong memory size requested.");

		size_t countElements = size / sizeof(T);
		size_t startingElement = allocatedCount;
		if (allocatedCount + countElements > N)
			throw std::exception("Memory manager out of memory.");
		auto ptr_start = allocatedCount;
		allocatedCount += countElements;
		return &this->memoryPool[ptr_start];
	}, (freeFunctor)[this](void* ptr) -> void {
		intptr_t diff = ((T*)ptr) - this->memoryPool;
		if(diff < 0 || diff > N)
			throw std::exception("Memory manager does not own the requested memory for deallocation.");
	}),
		allocatedCount(0)
	{
		std::fill(std::begin(memoryPool), std::end(memoryPool), T());
	}
	virtual ~MemoryManagerTN() {
	}
};

template<size_t chunkSize>
class MemoryManagerChunk : MemoryManagerRAII {
	uint8_t chunk[chunkSize];
	size_t currentOffset = 0;
public:
	MemoryManagerChunk()
		: MemoryManagerRAII((mallocFunctor)[this](size_t size) -> void* {
		if (!size)
			throw std::exception("Wrong memory size 0 requested.");
		if (currentOffset + size > chunkSize)
			throw std::exception("Memory manager out of memory.");
		auto ptr_start = currentOffset;
		currentOffset += size;
		return &this->chunk[ptr_start];
	}, (freeFunctor)[this](void* ptr) -> void {
		intptr_t diff = ((uint8_t*)ptr) - this->chunk;
		if (diff < 0 || diff > chunkSize)
			throw std::exception("Memory manager does not own the requested memory for deallocation.");
	}) {
		std::fill(std::begin(chunk), std::end(chunk), 0);
	}
};
#endif