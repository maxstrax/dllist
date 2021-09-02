#include "../include/cpp/dllist.hpp"

dllist_node* dllist_base_allocator(void* user_data) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	return d->alloc();
}
void dllist_base_deallocator(dllist_node* ptr, void* user_data) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	d->dealloc(ptr);
}
void dllist_base_initializer(dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	d->init(*dst, paramsCount, list);
}
void dllist_base_destructor(dllist_node* current, void* user_data) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	d->dest(*current);
}
int dllist_base_comparator(dllist_node *left, dllist_node *right, void* user_data) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	return d->cmp(*left, *right);
}
void dllist_base_copier(dllist_node* dst, dllist_node* src, void* user_data) {
	kgf::core::dllist* d = (kgf::core::dllist*)user_data;
	d->cp(*dst, *src);
}

namespace kgf {
	namespace core {
		dllist::dllist(dllistAllocator allocator, dllistDeallocator deallocator, dllistInitializer initializer, dllistDestructor destructor,
						dllistComparator comparator, dllistCopier copier) : ::dllist(), alloc(allocator), dealloc(deallocator), init(initializer),
																			dest(destructor), cmp(comparator), cp(copier) {
			dllist_init((::dllist*)this, ::dllist_base_allocator, ::dllist_base_deallocator, ::dllist_base_initializer, ::dllist_base_destructor, ::dllist_base_comparator, ::dllist_base_copier, this);
		}
		dllist::dllist(dllist_node_allocator allocate, dllist_node_deallocator deallocate, dllist_node_initializer initialize,
						dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data)
			: ::dllist(), alloc(nullptr), dealloc(nullptr), init(nullptr), dest(nullptr), cmp(nullptr), cp(nullptr)
		{
			dllist_init((::dllist*)this, allocate, deallocate, initialize, destroy, compare, copy, user_data);
		}
		dllist::dllist(dllist_node_allocator_custom allocate, dllist_node_deallocator deallocate, dllist_node_initializer initialize,
						dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data)
			: ::dllist(), alloc(nullptr), dealloc(nullptr), init(nullptr), dest(nullptr), cmp(nullptr), cp(nullptr)
		{
			dllist_init_custom((::dllist*)this, allocate, deallocate, initialize, destroy, compare, copy, user_data);
		}
		dllist::~dllist() {
			dllist_destroy((::dllist*)this);
		}

	}
}