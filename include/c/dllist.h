#ifndef DLLIST_H
#define DLLIST_H
	#ifdef __cplusplus
		extern "C" {
	#endif

			/*
			 * Doubly Linked List implementation for C.
			 */

			#include <stdint.h>
			#include <stdbool.h>
			#include <stdarg.h>

			// Base structure
			struct dllist_node {
				struct dllist_node *next, *previous;
			};

			// init node:
			typedef void(*dllist_node_initializer)(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list); // and initialize them directy from data
			typedef void (*dllist_node_copier)(struct dllist_node* dst, struct dllist_node* src, void* user_data); //used to perform a deep copy of the data
			typedef void(*dllist_node_destructor)(struct dllist_node* current, void* user_data);	//used to clear data from a node
			typedef int(*dllist_node_comparator)(struct dllist_node *left, struct dllist_node *right, void* user_data);	//used to compare values for find
			typedef void(*dllist_node_transform)(struct dllist_node* node, void* user_data);		// used to print the data of an individual node
			typedef struct dllist_node* (*dllist_node_allocator)(void* user_data); 	//used to allocate nodes 
			typedef struct dllist_node* (*dllist_node_allocator_custom)(void* user_data, size_t paramsCount, va_list list); 	//used to allocate nodes 
			typedef void(*dllist_node_deallocator)(struct dllist_node* ptr, void* user_data);	//used to deallocate nodes


			void dllist_node_init(struct dllist_node *current, struct dllist_node *next, struct dllist_node *previous); // initializes dllist_node only. Does not "insert" the node
			void dllist_node_init_null(struct dllist_node *current);

			// dllist_node_init_custom require the va_list in iniitializer to read the data read by custom_allocator as well
			void dllist_node_init_custom(struct dllist_node *current, struct dllist_node *next, struct dllist_node *previous, dllist_node_initializer initializer, void* user_data, uint8_t paramsCount, ...); // initializes dllist_node only. Does not "insert" the node

			void dllist_node_init_copy(struct dllist_node *dst, struct dllist_node *src);
			void dllist_node_init_deep_copy(struct dllist_node *dst, struct dllist_node *src, dllist_node_copier deep_copy, void* user_data);

			void dllist_node_destroy_custom(struct dllist_node *dst, dllist_node_destructor destructor, void* user_data);

			// Seek node:
			struct dllist_node* dllist_node_next(struct dllist_node *n); // the next node of n, if n is null, then nullptr is returned
			struct dllist_node* dllist_node_previous(struct dllist_node *n); // the previous node of n, if n is null, then nullptr is returned
			bool dllist_node_has_next(struct dllist_node *n); // true if n->next is not null, false otherwise
			bool dllist_node_has_previous(struct dllist_node *n); // true if n->previous is not null, false otherwise

			// Insert node:
			void dllist_node_insert_after(struct dllist_node *dst, struct dllist_node *node); // adds next to current->next. current->next->previous is not changed
			void dllist_node_insert_before(struct dllist_node *dst, struct dllist_node *node); // adds previous to current->previous. current->previous->next is not changed
			void dllist_node_insert_between(struct dllist_node *node, struct dllist_node *after, struct dllist_node *before); // CAUTION: This can cut nodes or bridge different lists

			// Remove node:
			void dllist_node_remove(struct dllist_node *node);
			void dllist_node_remove_after(struct dllist_node *node);
			void dllist_node_remove_before(struct dllist_node *node);

			// Size node:
			size_t dllist_node_size_after(struct dllist_node *node);
			size_t dllist_node_size_before(struct dllist_node *node);
			size_t dllist_node_size(struct dllist_node *node);
			size_t dllist_node_size_after_until(struct dllist_node *start, struct dllist_node *end);
			size_t dllist_node_size_before_until(struct dllist_node *start, struct dllist_node *end);

			// Find node:
			typedef bool (*dllist_node_find_stop)(struct dllist_node *n); //used to denote when to stop the search
			struct dllist_node* dllist_node_find_index_after(struct dllist_node *start, size_t index, dllist_node_find_stop stop);
			struct dllist_node* dllist_node_find_index_before(struct dllist_node *start, size_t index, dllist_node_find_stop stop);
			struct dllist_node* dllist_node_find_after(struct dllist_node *start, struct dllist_node *target, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop);
			struct dllist_node* dllist_node_find_before(struct dllist_node *start, struct dllist_node *target, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop);
			int dllist_node_find_num_nodes_after(struct dllist_node *start, struct dllist_node *end, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop);
			int dllist_node_find_num_nodes_before(struct dllist_node *start, struct dllist_node *end, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop);

			// transform node:
			void dllist_node_apply(struct dllist_node *node, dllist_node_transform transformation, void* user_data);

			// List:
			struct dllist {
				struct dllist_node head, tail;
				dllist_node_allocator allocate; //used to allocate nodes and initialize them directy from data
				dllist_node_allocator_custom allocate_custom;
				dllist_node_deallocator deallocate; //used to deallocate nodes
				dllist_node_initializer initialize;
				dllist_node_destructor destroy; //used to clear data from a node
				dllist_node_comparator compare; //used to compare values for find
				dllist_node_copier copy; //used to perform a deep copy of the data
				void* user_data; // data used by the user
			};

			// Init List
			void dllist_init(struct dllist *d, dllist_node_allocator allocate, dllist_node_deallocator deallocate,
				dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy,
				void* user_data);
			void dllist_init_custom(struct dllist *d, dllist_node_allocator_custom allocate, dllist_node_deallocator deallocate,
				dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy,
				void* user_data);
			void dllist_init_copy(struct dllist *dst, struct dllist *src);
			void dllist_init_move(struct dllist *dst, struct dllist *src);
			void dllist_destroy(struct dllist *dst);

			// Transform List:
			typedef void(*dllist_transform)(struct dllist*);
			void dllist_apply(struct dllist *d, dllist_transform transformation);
			void dllist_apply_all(struct dllist *d, dllist_transform list_transformation, dllist_node_transform node_transformation);

			// Container basics:
			bool dllist_last(struct dllist_node *n); // last data node on the list (->next points to tails)
			bool dllist_first(struct dllist_node *n); // first data node on the list(->previous points to head)
			bool dllist_end(struct dllist_node *n); //is tail?
			bool dllist_begin(struct dllist_node *n); //is head?
			bool dllist_empty(struct dllist *d);
			size_t dllist_size(struct dllist *d);
			
			// Manipulate Queue:
			struct dllist_node* dllist_peek_top(struct dllist *d); // first data node on the list, not head
			struct dllist_node* dllist_peek_bottom(struct dllist *d); // last data node on the list, not tail
			struct dllist_node* dllist_push_front(struct dllist *d, struct dllist_node * n);
			struct dllist_node* dllist_push_back(struct dllist *d, struct dllist_node *n);
			struct dllist_node* dllist_push_front_custom(struct dllist *d, uint8_t paramsCount, ...);
			struct dllist_node* dllist_push_back_custom(struct dllist *d, uint8_t paramsCount, ...);
			struct dllist_node* dllist_pop_front(struct dllist *d);
			struct dllist_node* dllist_pop_back(struct dllist *d);

			// Search
			struct dllist_node* dllist_find(struct dllist *d, struct dllist_node *target);
			bool dllist_exists(struct dllist *d, struct dllist_node *target); // 0 == not found
			size_t dllist_count_until(struct dllist *d, struct dllist_node *target);

			//Manipulate List
			struct dllist_node* dllist_get(struct dllist *d, size_t index); // returns null if index >= dllist_size(d)
			struct dllist_node* dllist_insert(struct dllist* d, struct dllist_node* node, size_t index);
			struct dllist_node* dllist_insert_sorted(struct dllist* d, struct dllist_node* node); // uses comparator to insert if present.
			struct dllist_node* dllist_remove(struct dllist* d, struct dllist_node* node); // removes first occurance of node and returns it. returns null if no occurance exist. does not destroy or deallocate
			struct dllist_node* dllist_remove_destroy(struct dllist* d, struct dllist_node* node); // removes and destroys first occurance of node and returns it. returns null if no occurance exist. does not deallocate
			void dllist_remove_deallocate(struct dllist* d, struct dllist_node* node); // removes, destroys and deallocates first occurance of node and returns it. returns null if no occurance exist.0
			size_t dllist_remove_times(struct dllist* d, struct dllist_node* node, size_t limit); // removes limit occurances of node, destroys and deallocates them. returns the amount of occurances removed.
			struct dllist_node* dllist_remove_at(struct dllist* d, size_t index); // returns null if index >= dllist_size(d)
			struct dllist_node* dllist_remove_destroy_at(struct dllist* d, size_t index); // returns null if index >= dllist_size(d)
			void dllist_remove_deallocate_at(struct dllist* d, size_t index); // removes, destroys and deallocates first occurance of node and returns it. returns null if no occurance exist.0

			// Container options
			void dllist_sort(struct dllist *d, bool asc);
			void dllist_clear(struct dllist *d);

			// data type based nodes
			struct dllist_node_void_ptr {
				struct dllist_node node;
				void* data;
			};
			struct dllist_node* dllist_node_void_ptr_allocate(void* user_data);
			void dllist_node_void_ptr_initializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list);
			void dllist_node_void_ptr_deallocate(struct dllist_node *n, void* user_data);
			int dllist_node_void_ptr_comparator(struct dllist_node *left, struct dllist_node *right, void* user_data);
			void dllist_node_void_ptr_copier(struct dllist_node* dst, struct dllist_node* src, void* user_data);
			void dllist_init_void_ptr(struct dllist *d, dllist_node_initializer init, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data);

			struct dllist_node_long {
				struct dllist_node node;
				long value;
			};
			struct dllist_node* dllist_node_long_allocate(void* user_data);
			void dllist_node_long_initializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list);
			void dllist_node_long_deallocate(struct dllist_node *n, void* user_data);
			int dllist_node_long_comparator(struct dllist_node *left, struct dllist_node *right, void* user_data);
			void dllist_node_long_copier(struct dllist_node* dst, struct dllist_node* src, void* user_data);
			void dllist_init_long(struct dllist *d, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data);

	#ifdef __cplusplus
		}
	#endif
#endif