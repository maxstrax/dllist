#include "../include/c/dllist.h"
#include "../include/c/mmalloc.h"

void dllist_node_init(struct dllist_node *current, struct dllist_node *next, struct dllist_node *previous) {
	if (!current)
		return;
	current->next = next;
	current->previous = previous;
}
void dllist_node_init_null(struct dllist_node *current) {
	dllist_node_init(current, NULL, NULL);
}
void dllist_node_init_custom(struct dllist_node *current, struct dllist_node *next, struct dllist_node *previous, dllist_node_initializer initializer, void* user_data, uint8_t paramsCount, ...) {
	dllist_node_init(current, next, previous);
	if (initializer) {
		va_list args;
		va_start(args, paramsCount);
		initializer(current, user_data, paramsCount, args);
		va_end(args);
	}
}
 void dllist_node_init_copy(struct dllist_node *dst, struct dllist_node *src) {
	if (!src)
		return;
	dllist_node_init(dst, src->next, src->previous);
}
 void dllist_node_init_deep_copy(struct dllist_node *dst, struct dllist_node *src, dllist_node_copier deep_copy, void* user_data) {
	dllist_node_init_copy(dst, src);
	if (deep_copy)
		deep_copy(dst, src, user_data);
}
 void dllist_node_destroy_custom(struct dllist_node *dst, dllist_node_destructor destructor, void* user_data) {
	if (dst && destructor)
		destructor(dst, user_data);
}

struct dllist_node* dllist_node_next(struct dllist_node* n) { return (n ? n->next : NULL); }
struct dllist_node* dllist_node_previous(struct dllist_node* n) { return (n ? n->previous : NULL); }
bool dllist_node_has_next(struct dllist_node *n) { return (bool)dllist_node_next(n); }
bool dllist_node_has_previous(struct dllist_node *n) { return (bool)dllist_node_previous(n); }

void dllist_node_insert_after(struct dllist_node *dst, struct dllist_node *node) {
	if (!dst)
		return;
	if (node) {
		node->previous = dst;
		node->next = dst->next;
	}
	if (dst->next)
		dst->next->previous = node;
	dst->next = node;
}
void dllist_node_insert_before(struct dllist_node *dst, struct dllist_node *node) {
	if (!dst)
		return;
	if (node) {
		node->next = dst;
		node->previous = dst->previous;
	}
	if (dst->previous)
		dst->previous->next = node;
	dst->previous = node;
}
void dllist_node_insert_between(struct dllist_node *node, struct dllist_node *after, struct dllist_node *before) {
	if (node) {
		node->next = before;
		node->previous = after;
	}
	if (after)
		after->next = node;
	if (before)
		before->previous = node;
}

void dllist_node_remove(struct dllist_node *node) {
	if (!node)
		return;
	if (node->next)
		node->next->previous = node->previous;
	if (node->previous)
		node->previous->next = node->next;
	node->next = node->previous = NULL;
}
void dllist_node_remove_after(struct dllist_node *node) {
	if (!node)
		return;
	dllist_node_remove(node->next);
}
void dllist_node_remove_before(struct dllist_node *node) {
	if (!node)
		return;
	dllist_node_remove(node->previous);
}

size_t dllist_node_size_after(struct dllist_node* node) {
	if (!node)
		return 0;
	struct dllist_node* n = node;
	size_t i = 1;
	while ((n = dllist_node_next(n)) && i < SIZE_MAX)
		i++;
	return i;
}
size_t dllist_node_size_before(struct dllist_node* node) {
	if (!node)
		return 0;
	struct dllist_node* n = node;
	size_t i = 1;
	while ((n = dllist_node_previous(n)))
		i++;
	return i;
}
size_t dllist_node_size(struct dllist_node *node) {
	if (!node)
		return 0;
	return dllist_node_size_after(node) + dllist_node_size_before(node) - 1;
}
size_t dllist_node_size_after_until(struct dllist_node *start, struct dllist_node *end) {
	if (!start || !end)
		return SIZE_MAX;
	struct dllist_node* n = start;
	size_t i = 1;
	while ((n = dllist_node_next(n)) && n != end)
		i++;
	return i;
}
size_t dllist_node_size_before_until(struct dllist_node *start, struct dllist_node *end) {
	if (!start || !end)
		return 0;
	struct dllist_node* n = start;
	size_t i = 1;
	while ((n = dllist_node_previous(n)) && n != end)
		i++;
	return i;
}

struct dllist_node* dllist_node_find_index_after(struct dllist_node* start, size_t index, dllist_node_find_stop stop) {
	struct dllist_node *current = start;
	while (current && !stop(current) && dllist_node_has_next(current) && index--) {
		current = dllist_node_next(current);
	}
	if (!++index)
		return current;
	return NULL;
}
struct dllist_node* dllist_node_find_index_before(struct dllist_node* start, size_t index, dllist_node_find_stop stop) {
	struct dllist_node *current = start;
	while (current && !stop(current) && dllist_node_has_previous(current) && index--) {
		current = dllist_node_previous(current);
	}
	if (!++index)
		return current;
	return NULL;
}
struct dllist_node* dllist_node_find_after(struct dllist_node *start, struct dllist_node *target, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop) {
	struct dllist_node *current = start;
	do {
		if (current == target)//if current.ptr == data.ptr then eventually they are the same, no need for comparisson
			return current;
		else if (!stop(current) && compare && compare(current, target, user_data) == 0) //if comparable, and *current ?= *data then current == data
			return current;
		current = dllist_node_next(current);
	} while (dllist_node_has_next(current));
	return NULL;
}
struct dllist_node* dllist_node_find_before(struct dllist_node *start, struct dllist_node *target, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop) {
	struct dllist_node *current = start;
	do {
		if (current == target)//if current.ptr == data.ptr then eventually they are the same, no need for comparisson
			return current;
		else if (!stop(current) && compare && !compare(current, target, user_data) == 0) //if comparable, and *current ?= *data then current == data
			return current;
		current = dllist_node_previous(current);
	} while (dllist_node_has_previous(current));
	return NULL;
}
int dllist_node_find_num_nodes_after(struct dllist_node *start, struct dllist_node *end, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop) {
	struct dllist_node* current = start;
	int i = 1;
	while (!stop(current) && dllist_node_has_next(current)) {
		if (current == end || (compare && compare(current, end, user_data) == 0))
			return i;
		i++;
		current = dllist_node_next(current);
	}
	return -1;
}
int dllist_node_find_num_nodes_before(struct dllist_node *start, struct dllist_node *end, dllist_node_comparator compare, void* user_data, dllist_node_find_stop stop) {
	struct dllist_node* current = start;
	int i = 1;
	while (!stop(current) && dllist_node_has_previous(current)) {
		current = dllist_node_previous(current);
		i++;
		if (start == end || (compare && compare(current, end, user_data) == 0))
			return i;
	}
	return -1;
}

void dllist_node_apply(struct dllist_node *node, dllist_node_transform transformation, void* user_data) {
	if (transformation)
		transformation(node, user_data);
}

void dllist_init(struct dllist *d, dllist_node_allocator allocate, dllist_node_deallocator deallocate, dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data) {
	if (!d)
		return;
	//connect head with tail, and set other ends to null
	d->tail.previous = &d->head;
	d->tail.next = NULL;
	d->head.next = &d->tail;
	d->head.previous = NULL;
	d->allocate = allocate;
	d->allocate_custom = NULL;
	d->deallocate = deallocate;
	d->initialize = initialize;
	d->destroy = destroy;
	d->compare = compare;
	d->copy = copy;
	d->user_data = user_data;
}
void dllist_init_custom(struct dllist *d, dllist_node_allocator_custom allocate, dllist_node_deallocator deallocate,
						dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy,
						void* user_data) {
	dllist_init(d, NULL, deallocate, initialize, destroy, compare, copy, user_data);
	d->allocate_custom = allocate;
}
struct dllist_node* allocateFunc(void* user_data) {
	return (struct dllist_node*)mmAlloc((size_t)user_data);
}
void dllist_init_copy(struct dllist *dst, struct dllist *src) {
	if (!src || !dst)
		return;
	dllist_init(dst, src->allocate, src->deallocate, src->initialize, src->destroy, src->compare, src->copy, src->user_data);
	dst->allocate_custom = src->allocate_custom; // this is not set.
	size_t count = dllist_size(src);
	size_t size = sizeof(struct dllist_node);
	struct dllist_node *add = NULL;
	struct dllist_node *current = &dst->head;
	struct dllist_node *n = dllist_peek_top(src);
	//setup allocator
	dllist_node_allocator alloc = allocateFunc;
	void* user_data = (void*)sizeof(struct dllist_node);
	if (dst->allocate) {
		alloc = dst->allocate;
		user_data = dst->user_data;
	}
	// copy each node
	for (size_t i = 0; i < count; i++) {
		add = alloc(user_data); //allocate
		if (dst->copy) {
			dst->copy(add, n, dst->user_data);
			n = dllist_node_next(n);
		}
		dllist_node_insert_between(add, current, current->next);
		current = add;
	}
}
void dllist_init_move(struct dllist *dst, struct dllist *src) {
	dllist_init(dst, src->allocate, src->deallocate, src->initialize, src->destroy, src->compare, src->copy, src->user_data);
	if (!dllist_empty(src)) {
		dst->head.next = src->head.next;
		dllist_node_insert_before(src->head.next, &dst->head);
		dllist_node_insert_after(src->tail.previous, &dst->tail);
	}; //else dst is ready
	dllist_init(src, src->allocate, src->deallocate, src->initialize, src->destroy, src->compare, src->copy, src->user_data);
}
void dllist_destroy(struct dllist *dst) {
	if (!dst)
		return;
	dllist_clear(dst);
}

void dllist_apply(struct dllist *d, dllist_transform transformation) {
	if (transformation)
		transformation(d);
}
void dllist_apply_all(struct dllist *d, dllist_transform list_transformation, dllist_node_transform node_transformation) {
	dllist_apply(d, list_transformation);
	if (node_transformation) {
		struct dllist_node* current = dllist_peek_top(d);
		while (!dllist_end(current)) {
			node_transformation(current, d->user_data);
			current = dllist_node_next(current);
		}
	}
}

bool dllist_first(struct dllist_node* n) { return !dllist_node_has_previous(n) || !dllist_node_has_previous(dllist_node_previous(n)); }
bool dllist_last(struct dllist_node* n) { return !dllist_node_has_next(n) || !dllist_node_has_next(dllist_node_next(n)); }
bool dllist_end(struct dllist_node* n) { return !dllist_node_has_next(n); }
bool dllist_begin(struct dllist_node* n) { return !dllist_node_has_previous(n); }
bool dllist_empty(struct dllist *d) { return dllist_node_next(&d->head) == &d->tail; };
size_t dllist_size(struct dllist * d) {
	if (d)
		return dllist_node_size_after(dllist_peek_top(d)) - 1;
	return 0;
}

struct dllist_node* dllist_peek_top(struct dllist * d) { return dllist_node_next(&d->head); }
struct dllist_node* dllist_peek_bottom(struct dllist * d) { return dllist_node_previous(&d->tail); }
struct dllist_node* dllist_push_front(struct dllist * d, struct dllist_node* n) {
	dllist_node_insert_after(&d->head, n);
	return n;
}
struct dllist_node* dllist_push_back(struct dllist * d, struct dllist_node* n) {
	dllist_node_insert_before(&d->tail, n);
	return n;
}
struct dllist_node* dllist_push_front_custom(struct dllist *d, uint8_t paramsCount, ...) {
	if (!d->initialize)
		return NULL;
	struct dllist_node* node;
	va_list l;
	va_start(l, paramsCount);

	if (d->allocate_custom) {
		node = d->allocate_custom(d->user_data, paramsCount, l);
		if (paramsCount)
			paramsCount--;
	} else if (d->allocate) {
		node = d->allocate(d->user_data);
	} else
		node = (struct dllist_node*)mmAlloc(sizeof(struct dllist_node));

	d->initialize(node, d->user_data, paramsCount, l);
	va_end(l);

	return dllist_push_front(d, node);
}
struct dllist_node* dllist_push_back_custom(struct dllist *d, uint8_t paramsCount, ...) {
	if (!d->initialize)
		return NULL;
	struct dllist_node* node;
	va_list l;
	va_start(l, paramsCount);
	if (d->allocate_custom) {
		node = d->allocate_custom(d->user_data, paramsCount, l);
	} else if (d->allocate) {
		node = d->allocate(d->user_data);
	} else
		node = (struct dllist_node*)mmAlloc(sizeof(struct dllist_node));

	d->initialize(node, d->user_data, paramsCount, l);
	va_end(l);

	return dllist_push_back(d, node);
}
struct dllist_node* dllist_pop_front(struct dllist * d) {
	struct dllist_node* n = dllist_peek_top(d);
	dllist_node_remove_after(&d->head);
	return n;
}
struct dllist_node* dllist_pop_back(struct dllist * d) {
	struct dllist_node* n = dllist_peek_bottom(d);
	dllist_node_remove_before(&d->tail);
	return n;
}

struct dllist_node* dllist_find(struct dllist *d, struct dllist_node *target) {
	if (!d || !target)
		return NULL;
	return dllist_node_find_after(d->head.next, target, d->compare, d->user_data, dllist_end);
}
bool dllist_exists(struct dllist *d, struct dllist_node *target) {
	return dllist_find(d, target); // != NULL
}
size_t dllist_count_until(struct dllist *d, struct dllist_node *target) {
	int result = dllist_node_find_num_nodes_after(dllist_peek_top(d), target, d->compare, d->user_data, dllist_end);
	if (result == -1)
		return SIZE_MAX;
	return (size_t)result;
}

struct dllist_node* dllist_get(struct dllist *d, size_t index) {
	if (dllist_empty(d))
		return NULL;
	return dllist_node_find_index_after(dllist_peek_top(d), index, dllist_end);
}
struct dllist_node* dllist_insert(struct dllist* d, struct dllist_node* node, size_t index) {
	struct dllist_node* found = dllist_get(d, index);
	if (!found) {
		found = &d->tail;
	}
	struct dllist_node* previous = found->previous;
	dllist_node_insert_before(found, node);
	if (previous)
		dllist_node_insert_after(previous, node);
	return node;
}
struct dllist_node* dllist_insert_sorted(struct dllist *dst, struct dllist_node* src) {
	if (!dst->compare || dllist_empty(dst)) {				//if the list is empty,
															//or no compare is present
		return dllist_push_back(dst, src);;					//push_back
	}

	struct dllist_node* n = dllist_peek_top(dst);			//iterate through the list
	while (!dllist_end(n) && dst->compare(n, src, dst->user_data) < 0)	//comparing while compare<0
		n = dllist_node_next(n);									//when the condition fails
	dllist_node_insert_before(n, src);	//the position will be between that node and the previous
	return src;
}
struct dllist_node* dllist_remove(struct dllist* d, struct dllist_node* node) {
	if (dllist_empty(d))
		return NULL;
	struct dllist_node *current = dllist_node_find_after(d->head.next, node, d->compare, d->user_data, dllist_end);
	if (!dllist_end(current)) {
		dllist_node_remove(current);
		return current;
	}
	return NULL;
}
struct dllist_node* dllist_remove_destroy(struct dllist* d, struct dllist_node* node) {
	if (dllist_empty(d))
		return NULL;
	struct dllist_node *current = dllist_node_find_after(d->head.next, node, d->compare, d->user_data, dllist_end);
	if (!dllist_end(current)) {
		dllist_node_remove(current);
		d->destroy(current, d->user_data);
		return current;
	}
	return NULL;
}
void dllist_remove_deallocate(struct dllist* d, struct dllist_node* node) {
	if (dllist_empty(d))
		return;
	struct dllist_node *current = dllist_node_find_after(d->head.next, node, d->compare, d->user_data, dllist_end);
	if (!dllist_end(current)) {
		dllist_node_remove(current);
		d->destroy(current, d->user_data);
		d->deallocate(node, d->user_data);
	}
}
size_t dllist_remove_times(struct dllist * d, struct dllist_node* data, size_t limit) {
	struct dllist_node *current = dllist_peek_top(d), *next = NULL;
	size_t i = 0;
	while (!dllist_end(current) && i<limit) {
		current = dllist_node_find_after(current, data, d->compare, d->user_data, dllist_end);
		if (!dllist_end(current)) {
			next = dllist_node_next(current);
			if (d->destroy)
				d->destroy(current, d->user_data);
			dllist_node_remove(current);
			if (d->deallocate)
				d->deallocate(current, d->user_data);
			current = next;
			i++;
		}
		else
			return i;
	}
	return i;
}
struct dllist_node* dllist_remove_at(struct dllist* d, size_t index) {
	if (dllist_empty(d))
		return NULL;
	struct dllist_node* found = dllist_get(d, index);
	if (found && found != &d->tail) {
		dllist_node_remove_after(found->previous);
		return found;
	}
	return NULL;
}
struct dllist_node* dllist_remove_destroy_at(struct dllist* d, size_t index) {
	if (dllist_empty(d))
		return NULL;
	struct dllist_node* found = dllist_get(d, index);
	if (found && found != &d->tail) {
		dllist_node_remove_after(found->previous);
		d->destroy(found, d->user_data);
		return found;
	}
	return NULL;
}
void dllist_remove_deallocate_at(struct dllist* d, size_t index) {
	if (dllist_empty(d))
		return;
	struct dllist_node* found = dllist_get(d, index);
	if (found && found != &d->tail) {
		dllist_node_remove_after(found->previous);
		d->destroy(found, d->user_data);
		d->deallocate(found, d->user_data);
	}
}

void dllist_sort(struct dllist * d, bool asc) {
	if (dllist_empty(d) || !d->compare)
		return;
	struct dllist_node *start, *current;
	int compare;
	start = dllist_peek_top(d);
	while (!dllist_end(start)) {
		current = start;
		while (!dllist_end(current)) {
			compare = d->compare(current, start, d->user_data);
			if ((compare < 0 && asc) || (compare > 0 && !asc))
				;//dllist_swap(start, current);
			current = dllist_node_next(current);
		}
		start = dllist_node_next(start);
	}
}
void dllist_clear(struct dllist *d) {
	struct dllist moved;
	dllist_init_move(&moved, d);
	struct dllist_node* current = dllist_peek_top(d);
	while (!dllist_end(current)) {
		if (d->destroy)
			d->destroy(current, d->user_data);
		if (d->deallocate)
			d->deallocate(current, d->user_data);
		current = dllist_node_next(current);
	}
}


struct dllist_node* dllist_node_void_ptr_allocate(void* user_data) {
	return (struct dllist_node*)mmAlloc(sizeof(struct dllist_node_void_ptr));
}
void dllist_node_void_ptr_initializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
	if (!paramsCount)
		return;
	((struct dllist_node_void_ptr*)dst)->data = va_arg(list, void*);
}
void dllist_node_void_ptr_deallocate(struct dllist_node *n, void* user_data) {
	mmFree((struct dllist_node_void_ptr*)n);
}
int dllist_node_void_ptr_comparator(struct dllist_node *left, struct dllist_node *right, void* user_data) {
	return (int)(((size_t)((struct dllist_node_void_ptr*)left)->data) - ((size_t)((struct dllist_node_void_ptr*)right)->data));
}
void dllist_node_void_ptr_copier(struct dllist_node* dst, struct dllist_node* src, void* user_data) {
	((struct dllist_node_void_ptr*)dst)->data = ((struct dllist_node_void_ptr*)src)->data;
}
void dllist_init_void_ptr(struct dllist *d, dllist_node_initializer init, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data) {
	if (!copy)
		copy = dllist_node_void_ptr_copier;
	if (!init)
		init = dllist_node_void_ptr_initializer;
	if (!compare)
		compare = dllist_node_void_ptr_comparator;
	dllist_init(d, dllist_node_void_ptr_allocate, dllist_node_void_ptr_deallocate, init, destroy, compare, copy, user_data);
}

struct dllist_node* dllist_node_long_allocate(void* user_data) {
	return (struct dllist_node*)mmAlloc(sizeof(struct dllist_node_long));
}
void dllist_node_long_initializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
	if (!paramsCount)
		return;
	((struct dllist_node_long*)dst)->value = va_arg(list, long);;
}
void dllist_node_long_deallocate(struct dllist_node *n, void* user_data) {
	mmFree((struct dllist_node_long*)n);
}
int dllist_node_long_comparator(struct dllist_node *left, struct dllist_node *right, void* user_data) {
	return ((struct dllist_node_long*)left)->value - ((struct dllist_node_long*)right)->value;
}
void dllist_node_long_copier(struct dllist_node* dst, struct dllist_node* src, void* user_data) {
	((struct dllist_node_long*)dst)->value = ((struct dllist_node_long*)src)->value;
}
void dllist_init_long(struct dllist *d, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy, void* user_data) {
	if (!copy)
		copy = dllist_node_long_copier;
	if (!compare)
		compare = dllist_node_long_comparator;
	dllist_init(d, dllist_node_long_allocate, dllist_node_long_deallocate, dllist_node_long_initializer, destroy, compare, copy, user_data);
}
