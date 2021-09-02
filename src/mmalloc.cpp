#include "../include/cpp/mmalloc.hpp"
#include <new>

//internal hack to allow functors, please do not use
mallocFunctor& mallocFunction() {
	static mallocFunctor functor;
	return functor;
};
freeFunctor& freeFunction() {
	static freeFunctor functor;
	return functor;
};

void* mallocFunctionHandler(size_t size) { return mallocFunction()(size); }
void freeFunctionHandler(void* ptr) { freeFunction()(ptr); }
//Overloads to set memory callbacks as functors, please use these
void setMallocCallback(mallocFunctor callback) {
	mallocFunction() = callback;
	setMallocCallback(mallocFunctionHandler);
};
void setFreeCallback(freeFunctor callback) {
	freeFunction() = callback;
	setFreeCallback(freeFunctionHandler);
}

// Overloads of new and delete
//void* operator new(std::size_t size) {
//	return mmAlloc(size);
//}
//void* operator new[](std::size_t size) {
//	return mmArrayAlloc(1, size, false);
//}
//void operator delete(void* ptr) {
//	mmFree(ptr);
//}
//void operator delete[](void* ptr) {
//	mmFree(ptr);
//}

MemoryManagerData::MemoryManagerData()
	: allocator(nullptr), allocationCallback(malloc), deallocator(nullptr), deallocationCallback(free)
{}
MemoryManagerData::MemoryManagerData(mallocFunctor Allocator, mallocCallback AllocationCallback, freeFunctor Deallocator, freeCallback DeallocationCallback)
	: allocator(Allocator), allocationCallback(AllocationCallback), deallocator(Deallocator), deallocationCallback(DeallocationCallback)
{}
bool MemoryManagerData::isUsed() {
	constexpr mallocCallback defaultMalloc = malloc;
	constexpr freeCallback defaultFree = free;
	bool isDefault = ((this->allocationCallback == defaultMalloc) && (this->deallocationCallback == defaultFree));
	if (!isDefault)
		return this->allocationCallback || this->allocator || this->deallocationCallback || this->deallocator;
	else
		return false;
}

MemoryManager::MemoryManager(mallocFunctor allocator, freeFunctor deallocator)
	: data{ allocator, mallocFunctionHandler, deallocator, freeFunctionHandler },
	previousData{}
{}
MemoryManager::MemoryManager(mallocCallback allocator, freeCallback deallocator)
	: data{ nullptr, allocator, nullptr, deallocator },
	previousData{}
{}

MemoryManager::MemoryManager(MemoryManagerData newData)
	: data( newData ),
	previousData{}
{}
MemoryManager::~MemoryManager() {}
void MemoryManager::takeOver() {
	if (this->previousData.isUsed())
		this->pullBack();
	this->previousData.allocationCallback = getMallocCallback();
	this->previousData.deallocationCallback = getFreeCallback();
	this->previousData.allocator = mallocFunction();
	this->previousData.deallocator = freeFunction();
	if (this->data.allocator)
		setMallocCallback(this->data.allocator);
	else
		setMallocCallback(this->data.allocationCallback);
	if (this->data.deallocator)
		setFreeCallback(this->data.deallocator);
	else
		setFreeCallback(this->data.deallocationCallback);
}
void MemoryManager::pullBack() {
	MemoryManager(this->previousData).takeOver();
	this->previousData = MemoryManagerData();
}

MemoryManagerRAII::MemoryManagerRAII(mallocFunctor allocator, freeFunctor deallocator)
	: MemoryManager(allocator, deallocator)
{
	this->takeOver();
}
MemoryManagerRAII::MemoryManagerRAII(mallocCallback allocator, freeCallback deallocator)
	: MemoryManager(allocator, deallocator)
{
	this->takeOver();
}
MemoryManagerRAII::MemoryManagerRAII(MemoryManagerData newData)
	: MemoryManager(newData)
{
	this->takeOver();
}
MemoryManagerRAII::~MemoryManagerRAII() {
	this->pullBack();
}

MemoryManager0::MemoryManager0() : MemoryManagerRAII(
	(mallocFunctor)[](size_t size)->void* {
		throw std::exception("A Memory allocation has been requested when allocations are prohibited");
	}, (freeFunctor)[](void* ptr) -> void {
		throw std::exception("A Memory deallocation has been requested when deallocations are prohibited");
	}) {};

MemoryManagerCpp::MemoryManagerCpp() : MemoryManagerRAII(
(mallocCallback)[](size_t size)->void* {
	return (void*)new uint8_t[size];
}, (freeCallback)[](void* ptr) -> void {
	delete[](uint8_t*)ptr;
}) {};

//MemoryManagerCpp defaultMemoryManager;