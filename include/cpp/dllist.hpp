#ifndef DLLIST_HPP
#define DLLIST_HPP

	#ifdef __cplusplus

		#include "../c/dllist.h"
		#include "../c/mmalloc.h"
		#include <functional>

		dllist_node* dllist_base_allocator(void* user_data);
		void dllist_base_deallocator(dllist_node* ptr, void* user_data);
		void dllist_base_initializer(dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list); // and initialize them directy from data
		void dllist_base_destructor(dllist_node* current, void* user_data);	//used to clear data from a node
		int dllist_base_comparator(dllist_node *left, dllist_node *right, void* user_data);	//used to compare values for find
		void dllist_base_copier(dllist_node* dst, dllist_node* src, void* user_data); //used to perform a deep copy of the data

		namespace kgf {
			namespace core {

				using dllistAllocator = std::function<dllist_node*()>;
				using dllistDeallocator = std::function<void(dllist_node*)>;
				using dllistInitializer = std::function<void(dllist_node&, uint8_t, va_list)>;
				using dllistDestructor = std::function<void(dllist_node&)>;
				using dllistComparator = std::function<int(const struct dllist_node&, const struct dllist_node&)>;
				using dllistCopier = std::function<void(struct dllist_node&, const struct dllist_node&)>;

				class dllist : public ::dllist {
				public:
					// friendlyness:
					friend dllist_node* ::dllist_base_allocator(void* user_data);
					friend void ::dllist_base_deallocator(dllist_node* ptr, void* user_data);
					friend void ::dllist_base_initializer(dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list);
					friend void ::dllist_base_destructor(dllist_node* current, void* user_data);
					friend int ::dllist_base_comparator(dllist_node *left, dllist_node *right, void* user_data);
					friend void ::dllist_base_copier(dllist_node* dst, dllist_node* src, void* user_data);
					
					// std::function< prototypes >
					dllistAllocator alloc;
					dllistDeallocator dealloc;
					dllistInitializer init;
					dllistDestructor dest;
					dllistComparator cmp;
					dllistCopier cp;
					dllist(dllistAllocator allocator = nullptr, dllistDeallocator deallocator = nullptr, dllistInitializer initializer = nullptr,
						   dllistDestructor destructor = nullptr, dllistComparator comparator = nullptr, dllistCopier copier = nullptr);
					dllist(dllist_node_allocator allocate, dllist_node_deallocator deallocate,
						dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy,
						void* user_data);
					dllist(dllist_node_allocator_custom allocate, dllist_node_deallocator deallocate,
						dllist_node_initializer initialize, dllist_node_destructor destroy, dllist_node_comparator compare, dllist_node_copier copy,
						void* user_data);
					~dllist();

					template<typename... Types>
					dllist_node* push_back_custom(Types... args) {
						return dllist_push_back_custom(this, sizeof...(args), std::forward<Types>(args)...);
					}

					dllist_node* push_back(const dllist_node& arg) {
						return dllist_push_back(this, (dllist_node*)&arg);
					}

					dllist_node* push_back(dllist_node* arg) {
						return dllist_push_back(this, arg);
					}
				};

				template <typename T>
				struct dllistNodeT : public ::dllist_node {
					T data;
				};

				template <typename T>
				class dllistT : public dllist {
				public:
					dllistT() : dllist(
						[](size_t count, size_t& size) -> dllist_node* {
							size = sizeof(dllistNodeT<T>);
							return mmAlloc(size * count);
						},
						[](dllist_node* ptr, size_t count) {
							mmFree((dllistNodeT<T>*) ptr);
						},
						[](dllist_node& node, uint8_t paramCount, va_list l) {
							if (paramCount == 1) {
								dllistNodeT<T>* nodeT = (dllistNodeT<T>*) node;
								nodeT->data = va_arg(l, T);
							}
						},
						[](dllist_node& node) {
							dllistNodeT<T>* nodeT = (dllistNodeT<T>*) node;
							nodeT->data = T();
						},
						[](const struct dllist_node& lhs, const struct dllist_node& rhs) -> int {
							dllistNodeT<T> &left = lhs, &right = rhs;
							if (std::equal(left, rhs))
								return 0;
							else if (std::less(left, rhs))
								return 1;
							else
								return -1;
						},
						[](struct dllist_node& dst, const struct dllist_node& src) {
							dllistNodeT<T> &left = dst, &right = src;
							left.data = right.data;
						}
					){}
				};
			}
			
		}

	#endif
#endif