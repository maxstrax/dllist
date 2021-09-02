#ifndef CUSTOM_MALLOC_H
#define CUSTOM_MALLOC_H
	#ifdef __cplusplus
		extern "C" {
	#endif

			#include <string.h>
			#include <stdbool.h>

			// memory manager allocate
			void* mmArrayAlloc(size_t count, size_t size, bool init0);
			void* mmAllocInit(size_t size, bool init0);
			void* mmAlloc(size_t size);
			void mmFree(void* ptr);

			typedef void* (*mallocCallback)(size_t);
			typedef void (*freeCallback)(void*);

			mallocCallback getMallocCallback();
			void setMallocCallback(mallocCallback callback);
			freeCallback getFreeCallback();
			void setFreeCallback(freeCallback callback);
			void resetMemoryCallbacks();

	#ifdef __cplusplus
		}
	#endif
#endif