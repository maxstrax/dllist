#include "../include/c/mmalloc.h"
#include <malloc.h>

void* (*mmAllocMalloc)(size_t) = malloc;
void(*mmAllocFree)(void*) = free;


void* mmArrayAlloc(size_t count, size_t size, bool init0) {
	size_t actualSize = count * size;
	if (actualSize) { // | and  || will have the same effect when converting to bool, only 0 | 0 == 0
		void* result = mmAllocMalloc(actualSize);
		if (init0) {
			return memset(result, 0, actualSize);
		}
		return result;
	}
	return NULL;
}
void* mmAllocInit(size_t size, bool init0) {
	void* result = mmAlloc(size);
	if (init0 && result) {
		memset(result, 0, size);
	}
	return result;
}
void* mmAlloc(size_t size) {
	if (size) {
		return mmAllocMalloc(size);
	}
	return NULL;
}
void mmFree(void* ptr) {
	if (ptr)
		mmAllocFree(ptr);
}

mallocCallback getMallocCallback() {
	return mmAllocMalloc;
}

void setMallocCallback(mallocCallback callback) {
	mmAllocMalloc = callback;
}

freeCallback getFreeCallback() {
	return mmAllocFree;
}

void setFreeCallback(freeCallback callback) {
	mmAllocFree = callback;
}

void resetMemoryCallbacks() {
	setMallocCallback(malloc);
	setFreeCallback(free);
}