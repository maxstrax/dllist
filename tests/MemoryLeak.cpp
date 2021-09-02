#include "MemoryLeak.h"
//#include "../include/c/mmalloc.h"
//#include <stdint.h>
//#include <new>
//
//struct memoryChunkData {
//	bool isDeleted;
//	size_t length;
//	size_t count; // length /count = cell size
//	memoryChunkData(bool deleted, size_t size, size_t elementsCount) {
//		this->isDeleted = deleted;
//		this->length = size;
//	}
//};
//
//struct memoryChunkGuard {
//	uint64_t pattern;
//	memoryChunkGuard(uint64_t leakPatttern) {
//		this->pattern = leakPatttern;
//	}
//};
//
//#define Add01(value) ((value) << 2) | 0x01
//#define PtrOffset(ptr, offset) ((void*)(((size_t)(ptr)) + ((size_t)(offset))))
//
//const uint64_t leak_pattern = Add01(Add01(Add01(Add01(Add01(Add01(Add01(Add01(0))))))));
//
//void* test_malloc(size_t size) {
//	void* ptr = new uint8_t[size + sizeof(memoryChunkData) + 2* sizeof(memoryChunkGuard)];
//	new((memoryChunkData*)ptr) memoryChunkData(true, size, 1); // initialize the data about the allocation
//	new((memoryChunkData*)PtrOffset(ptr, sizeof(memoryChunkData))) memoryChunkGuard(leak_pattern); // initialize underflow leak detection
//	new((memoryChunkData*)PtrOffset(ptr, sizeof(memoryChunkData) + sizeof(memoryChunkGuard) + size)) memoryChunkGuard(leak_pattern); // initialize underflow leak detection
//	return PtrOffset(ptr, sizeof(memoryChunkData) + sizeof(memoryChunkGuard));
//}
//
//void test_free(void* ptr) {
//	delete (uint8_t*)ptr;
//}
//
//bool doesNotHaveUnderFlow(void* ptr) {
//	memoryChunkGuard* guard = (memoryChunkGuard*)PtrOffset(ptr, sizeof(memoryChunkData));
//	return guard->pattern == leak_pattern;
//}
//
//bool doesNotHaveOverFlow(void* ptr) {
//	size_t size = ((memoryChunkData*)ptr)->length; //get the size
//	memoryChunkGuard* guard = (memoryChunkGuard*)PtrOffset(ptr, sizeof(memoryChunkData) + sizeof(memoryChunkGuard) + size);
//	return guard->pattern == leak_pattern;
//}
//
//bool doesNotHaveViolations(void* ptr) {
//	return doesNotHaveUnderFlow(ptr) && doesNotHaveOverFlow(ptr);
//}
//
//void setupMemoryManager() {
//	setMallocCallback(test_malloc);
//	setFreeCallback(test_free);
//}