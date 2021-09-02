#ifndef CUSTOM_MALLOC_H_CPP
#define CUSTOM_MALLOC_H_CPP

	#include "../c/mmalloc.h"

	#ifdef __cplusplus

		#include <string.h>
		#include <functional>
		#include <new>
	
		using mallocFunctor = std::function<void* (size_t size)>;
		using freeFunctor = std::function<void(void* ptr)>;

		//internal hack to allow functors, please do not use
		mallocFunctor& mallocFunction();
		freeFunctor& freeFunction();

		//Overloads to set memory callbacks as functors, please use these
		void setMallocCallback(mallocFunctor callback);
		void setFreeCallback(freeFunctor callback);

		// Overloads of new and delete

		// memory manager classes
		struct MemoryManagerData {
			mallocFunctor allocator;
			mallocCallback allocationCallback;
			freeFunctor deallocator;
			freeCallback deallocationCallback;
			MemoryManagerData();
			MemoryManagerData(mallocFunctor Allocator, mallocCallback AllocationCallback, freeFunctor Deallocator, freeCallback DeallocationCallback);
			bool isUsed();
		};
		class MemoryManager {
			MemoryManagerData data;
			MemoryManagerData previousData;
		public:
			MemoryManager(mallocFunctor allocator, freeFunctor deallocator);
			MemoryManager(mallocCallback allocator, freeCallback deallocator);
			MemoryManager(MemoryManagerData newData);
			virtual ~MemoryManager();

			void takeOver(); // use this memoryManager instead of the current one
			void pullBack(); // restore the previous memoryManager
		};

		class MemoryManagerRAII : public MemoryManager {
		public:
			MemoryManagerRAII(mallocFunctor allocator, freeFunctor deallocator);
			MemoryManagerRAII(mallocCallback allocator, freeCallback deallocator);
			MemoryManagerRAII(MemoryManagerData newData);
			virtual ~MemoryManagerRAII();
		};

		class MemoryManager0 : public MemoryManagerRAII {
		public:
			MemoryManager0();
			~MemoryManager0() final = default;
		};

		class MemoryManagerCpp : public MemoryManagerRAII {
		public:
			MemoryManagerCpp();
			~MemoryManagerCpp() final = default;
		};

		//extern MemoryManagerCpp defaultMemoryManager;
	#endif
#endif