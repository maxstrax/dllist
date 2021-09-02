#include <memory.h>
#include "../include/c/dllist.h"
#include "../include/cpp/mmalloc.hpp"
#include "MemoryLeak.h"

#include <exception>

#include <gtest\gtest.h>


class dllistTests : public ::testing::Test {
protected:
	std::function<void(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list)> initializer;
	std::function<void(struct dllist_node* dst, struct dllist_node* src, void* user_data)> copier;
public:
	void runInitializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
		this->initializer(dst, user_data, paramsCount, list);
	}
	void runCopier(struct dllist_node* dst, struct dllist_node* src, void* user_data) {
		this->copier(dst, src, user_data);
	}

		void SetUp() final {
		resetMemoryCallbacks();
	}

	void TearDown() final {
		resetMemoryCallbacks();
	}
};
void testFixtureDllistInitializer(struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
	dllistTests* tf = (dllistTests*)user_data;
	tf->runInitializer(dst, user_data, paramsCount, list);
};
void testFixtureDllistCopier(struct dllist_node* dst, struct dllist_node* src, void* user_data) {
	dllistTests* tf = (dllistTests*)user_data;
	tf->runCopier(dst, src, user_data);
};

TEST_F(dllistTests, dllist_node_init_InitsTheStruct) {
	//Testing null item:
	dllist_node_init(nullptr, (dllist_node*)1, (dllist_node*)2);
	dllist_node_init(nullptr, nullptr, (dllist_node*)2);
	dllist_node_init(nullptr, (dllist_node*)1, nullptr);
	dllist_node_init(nullptr, nullptr, nullptr);

	dllist_node node;

	// ASSIGN non-null, non-null
	memset(&node, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init(&node, (dllist_node*)1, (dllist_node*)2);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);

	// ASSIGN null, non-null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init(&node, nullptr, (dllist_node*)1);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);


	// ASSIGN non-null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init(&node, (dllist_node*)1, nullptr);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);


	// ASSIGN null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init(&node, nullptr, nullptr);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
}
TEST_F(dllistTests, dllist_node_init_null_InitsTheStruct) {
	// Test null items:
	dllist_node_init_null(nullptr);
	
	dllist_node node;
	// ASSIGN
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT
	dllist_node_init_null(&node);

	// ASSERT
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
}
TEST_F(dllistTests, dllist_node_init_custom_WithoutInitializerInitsTheStruct) {
	// Test null item
	dllist_node_init_custom(nullptr, (dllist_node*)1, (dllist_node*)2, nullptr, nullptr, 0);
	dllist_node_init_custom(nullptr, nullptr, (dllist_node*)2, nullptr, nullptr, 0);
	dllist_node_init_custom(nullptr, (dllist_node*)1, nullptr, nullptr, nullptr, 0);
	dllist_node_init_custom(nullptr, nullptr, nullptr, nullptr, nullptr, 0);

	dllist_node node;

	// ASSIGN non-null, non-null
	memset(&node, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init_custom(&node, (dllist_node*)1, (dllist_node*)2, nullptr, nullptr, 0);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);

	// ASSIGN null, non-null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init_custom(&node, nullptr, (dllist_node*)1, nullptr, nullptr, 0);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);


	// ASSIGN non-null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init_custom(&node, (dllist_node*)1, nullptr, nullptr, nullptr, 0);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);


	// ASSIGN null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init_custom(&node, nullptr, nullptr, nullptr, nullptr, 0);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
}
TEST_F(dllistTests, dllist_node_init_custom_WithInitializerInitsTheStructAndCallsInitializer) {
	dllist_node node;
	int value = 14;
	bool hasCalledInitializer = false;
	this->initializer = [&hasCalledInitializer, &node, value](struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
		hasCalledInitializer = true;
		ASSERT_FALSE(dst);
		ASSERT_EQ(paramsCount, 1);
		int v;
		v = va_arg(list, int);
		EXPECT_EQ(v, value);
	};

	// Test null item
	dllist_node_init_custom(nullptr, (dllist_node*)1, (dllist_node*)2, testFixtureDllistInitializer, this, 1, value);
	dllist_node_init_custom(nullptr, nullptr, (dllist_node*)2, testFixtureDllistInitializer, this, 1, value);
	dllist_node_init_custom(nullptr, (dllist_node*)1, nullptr, testFixtureDllistInitializer, this, 1, value);
	dllist_node_init_custom(nullptr, nullptr, nullptr, testFixtureDllistInitializer, this, 1, value);

	this->initializer = [&hasCalledInitializer, &node, value](struct dllist_node* dst, void* user_data, uint8_t paramsCount, va_list list) {
		hasCalledInitializer = true;
		ASSERT_EQ(dst, &node);
		ASSERT_EQ(paramsCount, 1);
		int v;
		v = va_arg(list, int);
		EXPECT_EQ(v, value);
	};

	// ASSIGN non-null, non-null
	memset(&node, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init_custom(&node, (dllist_node*)1, (dllist_node*)2, testFixtureDllistInitializer, this, 1, value);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);
	EXPECT_TRUE(hasCalledInitializer); // if the initializer has been called then the rest can be checked
	hasCalledInitializer = false;

	// ASSIGN null, non-null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init_custom(&node, nullptr, (dllist_node*)1, testFixtureDllistInitializer, this, 1, value);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);
	EXPECT_TRUE(hasCalledInitializer); // if the initializer has been called then the rest can be checked
	hasCalledInitializer = false;

	// ASSIGN non-null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init_custom(&node, (dllist_node*)1, nullptr, testFixtureDllistInitializer, this, 1, value);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_TRUE(hasCalledInitializer); // if the initializer has been called then the rest can be checked
	hasCalledInitializer = false;

	// ASSIGN null, null
	memset(&node, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init_custom(&node, nullptr, nullptr, testFixtureDllistInitializer, this, 1, value);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_TRUE(hasCalledInitializer); // if the initializer has been called then the rest can be checked
	hasCalledInitializer = false;
}
TEST_F(dllistTests, dllist_node_init_copy_copiesTheStruct) {
	dllist_node node, copied;

	// Test null object
	dllist_node_init_copy(nullptr, nullptr);
	dllist_node_init_copy(nullptr, &node);

	// Test null src
	memset(&copied, 0, sizeof(dllist_node));
	dllist_node_init_copy(&copied, nullptr);
	EXPECT_EQ(copied.next, nullptr);
	EXPECT_EQ(copied.previous, nullptr);

	// ASSIGN non-null, non-null
	memset(&copied, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init(&node, (dllist_node*)1, (dllist_node*)2);
	dllist_node_init_copy(&copied, &node);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);

	// ASSIGN null, non-null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init(&node, nullptr, (dllist_node*)1);
	dllist_node_init_copy(&copied, &node);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);


	// ASSIGN non-null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init(&node, (dllist_node*)1, nullptr);
	dllist_node_init_copy(&copied, &node);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);

	// ASSIGN null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init(&node, nullptr, nullptr);
	dllist_node_init_copy(&copied, &node);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
}
TEST_F(dllistTests, dllist_node_init_deep_copy_WithoutCopierCopiesTheStruct) {
	dllist_node node, copied;

	//Test null object:
	dllist_node_init_deep_copy(nullptr, &node, nullptr, nullptr);
	dllist_node_init_deep_copy(&copied, nullptr, nullptr, nullptr);

	// ASSIGN non-null, non-null
	memset(&copied, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init(&node, (dllist_node*)1, (dllist_node*)2);
	dllist_node_init_deep_copy(&copied, &node, nullptr, nullptr);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);

	// ASSIGN null, non-null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init(&node, nullptr, (dllist_node*)1);
	dllist_node_init_deep_copy(&copied, &node, nullptr, nullptr);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);

	// ASSIGN non-null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init(&node, (dllist_node*)1, nullptr);
	dllist_node_init_deep_copy(&copied, &node, nullptr, nullptr);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);

	// ASSIGN null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init(&node, nullptr, nullptr);
	dllist_node_init_deep_copy(&copied, &node, nullptr, nullptr);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
}
TEST_F(dllistTests, dllist_node_init_deep_copy_copiesTheStructAndCallsCopier) {
	dllist_node node, copied;
	bool hasCalledCopier = false;
	this->copier = [&hasCalledCopier, &node, &copied](struct dllist_node* dst, struct dllist_node* src, void* user_data) {
		hasCalledCopier = true;
		ASSERT_EQ(dst, &copied);
		ASSERT_EQ(src, &node);
	};

	// ASSIGN non-null, non-null
	memset(&copied, 0, sizeof(dllist_node));

	// ACT non-null, non-null
	dllist_node_init(&node, (dllist_node*)1, (dllist_node*)2);
	dllist_node_init_deep_copy(&copied, &node, testFixtureDllistCopier, this);

	// ASSERT non-null, non-null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, (dllist_node*)2);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
	EXPECT_TRUE(hasCalledCopier); // if the copier has been called then the rest can be checked
	hasCalledCopier = false;

	// ASSIGN null, non-null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, non-null
	dllist_node_init(&node, nullptr, (dllist_node*)1);
	dllist_node_init_deep_copy(&copied, &node, testFixtureDllistCopier, this);

	// ASSERT null, non-null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, (dllist_node*)1);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
	EXPECT_TRUE(hasCalledCopier); // if the copier has been called then the rest can be checked
	hasCalledCopier = false;

	// ASSIGN non-null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT non-null, null
	dllist_node_init(&node, (dllist_node*)1, nullptr);
	dllist_node_init_deep_copy(&copied, &node, testFixtureDllistCopier, this);

	// ASSERT non-null, null
	EXPECT_EQ(node.next, (dllist_node*)1);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
	EXPECT_TRUE(hasCalledCopier); // if the copier has been called then the rest can be checked
	hasCalledCopier = false;

	// ASSIGN null, null
	memset(&copied, 0xFFFFFFFF, sizeof(dllist_node));

	// ACT null, null
	dllist_node_init(&node, nullptr, nullptr);
	dllist_node_init_deep_copy(&copied, &node, testFixtureDllistCopier, this);

	// ASSERT null, null
	EXPECT_EQ(node.next, nullptr);
	EXPECT_EQ(node.previous, nullptr);
	EXPECT_EQ(copied.next, node.next);
	EXPECT_EQ(copied.previous, node.previous);
	EXPECT_TRUE(hasCalledCopier); // if the copier has been called then the rest can be checked
	hasCalledCopier = false;
}
TEST_F(dllistTests, dllist_node_next_ReturnsTheCorrectPtr) {
	dllist_node a, b, c;
	// Test Null item
	EXPECT_FALSE(dllist_node_next(nullptr));

	// ASSIGN:
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &a);
	dllist_node_init(&c, nullptr, &b);

	// ACT & ASSERT:
	EXPECT_EQ(dllist_node_next(&a), &b);
	EXPECT_EQ(dllist_node_next(&b), &c);
	EXPECT_FALSE(dllist_node_next(&c));
}
TEST_F(dllistTests, dllist_node_previous_ReturnsTheCorrectPtr) {
	dllist_node a, b, c;
	// Test Null item
	EXPECT_FALSE(dllist_node_previous(nullptr));

	// ASSIGN:
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &a);
	dllist_node_init(&c, nullptr, &b);

	// ACT & ASSERT:
	EXPECT_EQ(dllist_node_previous(&c), &b);
	EXPECT_EQ(dllist_node_previous(&b), &a);
	EXPECT_FALSE(dllist_node_previous(&a));
}
TEST_F(dllistTests, dllist_node_has_next_ReturnsTheCorrectPtr) {
	dllist_node a, b, c;
	// Test Null item
	EXPECT_FALSE(dllist_node_has_next(nullptr));

	// ASSIGN:
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &a);
	dllist_node_init(&c, nullptr, &b);

	// ACT & ASSERT:
	EXPECT_TRUE(dllist_node_has_next(&a));
	EXPECT_TRUE(dllist_node_has_next(&b));
	EXPECT_FALSE(dllist_node_has_next(&c));
}
TEST_F(dllistTests, dllist_node_has_previous_ReturnsTheCorrectPtr) {
	dllist_node a, b, c;
	// Test Null item
	EXPECT_FALSE(dllist_node_has_previous(nullptr));

	// ASSIGN:
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &a);
	dllist_node_init(&c, nullptr, &b);

	// ACT & ASSERT:
	EXPECT_TRUE(dllist_node_has_previous(&c));
	EXPECT_TRUE(dllist_node_has_previous(&b));
	EXPECT_FALSE(dllist_node_has_previous(&a));
}
TEST_F(dllistTests, dllist_node_insert_after_CorrectResults) {
	dllist_node a, b, c, d, node;

	// Test null object:
	dllist_node_insert_after(nullptr, &node);
	dllist_node_insert_after(nullptr, nullptr);

	// ASSIGN
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &b);
	dllist_node_init(&c, &d, &b);
	dllist_node_init(&d, nullptr, &c);

	memset(&node, 0, sizeof(dllist_node));

	// ACT 1
	dllist_node_insert_after(&a, &node);

	// ASSERT 1
	EXPECT_EQ(a.previous, nullptr);
	EXPECT_EQ(a.next, &node);
	EXPECT_EQ(b.previous, &node);
	EXPECT_EQ(b.next, &c);
	EXPECT_EQ(c.previous, &b);
	EXPECT_EQ(c.next, &d);
	EXPECT_EQ(d.previous, &c);
	EXPECT_EQ(d.next, nullptr);
	EXPECT_EQ(node.previous, &a);
	EXPECT_EQ(node.next, &b);

	// ACT 2
	dllist_node_insert_after(&c, nullptr);

	// ASSERT 2
	EXPECT_EQ(c.previous, &b);
	EXPECT_EQ(c.next, nullptr);
	EXPECT_EQ(d.previous, nullptr);
	EXPECT_EQ(d.next, nullptr);
}
TEST_F(dllistTests, dllist_node_insert_before_CorrectResults) {
	dllist_node a, b, c, d, node;

	// Test null object:
	dllist_node_insert_after(nullptr, &node);
	dllist_node_insert_after(nullptr, nullptr);

	// ASSIGN
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, &c, &a);
	dllist_node_init(&c, &d, &b);
	dllist_node_init(&d, nullptr, &c);
	memset(&node, 0, sizeof(dllist_node));

	// ACT 1
	dllist_node_insert_before(&d, &node);

	// ASSERT 1
	EXPECT_EQ(a.previous, nullptr);
	EXPECT_EQ(a.next, &b);
	EXPECT_EQ(b.previous, &a);
	EXPECT_EQ(b.next, &c);
	EXPECT_EQ(c.previous, &b);
	EXPECT_EQ(c.next, &node);
	EXPECT_EQ(d.previous, &node);
	EXPECT_EQ(d.next, nullptr);
	EXPECT_EQ(node.previous, &c);
	EXPECT_EQ(node.next, &d);

	// ACT 2
	dllist_node_insert_before(&b, nullptr);

	// ASSERT 2
	EXPECT_EQ(b.previous, nullptr);
	EXPECT_EQ(b.next, &c);
	EXPECT_EQ(a.previous, nullptr);
	EXPECT_EQ(a.next, nullptr);
}
TEST_F(dllistTests, dllist_node_insert_between_CorrectResults) {
	dllist_node a, b, node;
	dllist_node_init(&a, &b, nullptr);
	dllist_node_init(&b, nullptr, &b);
	memset(&node, 0, sizeof(dllist_node));

	dllist_node_insert_between(&node, &a, &b);

	EXPECT_EQ(a.previous, nullptr);
	EXPECT_EQ(a.next, &node);
	EXPECT_EQ(b.previous, &node);
	EXPECT_EQ(b.next, nullptr);
	EXPECT_EQ(node.previous, &a);
	EXPECT_EQ(node.next, &b);
}
TEST_F(dllistTests, dllist_node_remove_CorrectResults) {
	// Test Null item:
	dllist_node_remove(nullptr);

	dllist_node previous, current, next;
	dllist_node currentNoPrevious, next2;
	dllist_node previous2, currentNoNext;
	dllist_node sink;
	//TC1 Assign
	previous.next = &current;
	next.previous = &current;
	current.next = &next;
	current.previous = &previous;
	next.next = &sink;
	previous.previous = &sink;
	//TC2 Assign
	currentNoPrevious.previous = NULL;
	currentNoPrevious.next = &next2;
	next2.previous = &currentNoPrevious;
	next2.next = &sink;
	//TC3 Assign
	currentNoNext.next = NULL;
	currentNoNext.previous = &previous2;
	previous2.next = &currentNoNext;
	previous2.previous = &sink;

	//TC1 Act
	dllist_node_remove(&current);
	//TC2 Act
	dllist_node_remove(&currentNoNext);
	//TC3 Act
	dllist_node_remove(&currentNoPrevious);

	//TC1 Assert
	EXPECT_EQ(previous.next, &next);
	EXPECT_EQ(next.previous, &previous);
	EXPECT_EQ(current.previous, (dllist_node*)NULL);
	EXPECT_EQ(current.next, (dllist_node*)NULL);
	//TC2 Assert
	EXPECT_EQ(previous2.next, (dllist_node*)NULL);
	EXPECT_EQ(currentNoNext.previous, (dllist_node*)NULL);
	EXPECT_EQ(currentNoNext.next, (dllist_node*)NULL);
	//TC3 Assert
	EXPECT_EQ(next2.previous, (dllist_node*)NULL);
	EXPECT_EQ(currentNoPrevious.next, (dllist_node*)NULL);
	EXPECT_EQ(currentNoPrevious.previous, (dllist_node*)NULL);
}
TEST_F(dllistTests, dllist_node_remove_after) {
	// Test Null item:
	dllist_node_remove_after(nullptr);

	dllist_node previous, current, next;
	dllist_node previous2, currentNoNext;
	dllist_node sink;
	//TC1 Assign
	previous.next = &current;
	next.previous = &current;
	current.next = &next;
	current.previous = &previous;
	next.next = &sink;
	previous.previous = &sink;
	//TC2 Assign
	currentNoNext.next = NULL;
	currentNoNext.previous = &previous2;
	previous2.next = &currentNoNext;
	previous2.previous = &sink;

	//TC1 Act
	dllist_node_remove_after(&previous);
	//TC2 Act
	dllist_node_remove_after(&currentNoNext);

	//TC1 Assert
	EXPECT_EQ(previous.next, &next);
	EXPECT_EQ(next.previous, &previous);
	EXPECT_EQ(current.previous, (dllist_node*)NULL);
	EXPECT_EQ(current.next, (dllist_node*)NULL);
	//TC2 Assert
	EXPECT_EQ(previous2.next, (dllist_node*)&currentNoNext);
	EXPECT_EQ(currentNoNext.next, (dllist_node*)NULL);
	EXPECT_EQ(currentNoNext.previous, (dllist_node*)&previous2);
}
TEST_F(dllistTests, DISABLED_dllist_node_remove_before) {}
TEST_F(dllistTests, DISABLED_dllist_node_size_after) {}
TEST_F(dllistTests, DISABLED_dllist_node_size_before) {}
TEST_F(dllistTests, DISABLED_dllist_node_size) {}
TEST_F(dllistTests, DISABLED_dllist_node_size_after_until) {}
TEST_F(dllistTests, DISABLED_dllist_node_size_before_until) {}
TEST_F(dllistTests, DISABLED_dllist_node_find_index_after) {}
TEST_F(dllistTests, DISABLED_dllist_node_find_index_before) {}
TEST_F(dllistTests, DISABLED_dllist_node_find_after) {}
TEST_F(dllistTests, DISABLED_dllist_node_find_before) {}
TEST_F(dllistTests, DISABLED_dllist_find_num_nodes_after) {}
TEST_F(dllistTests, DISABLED_dllist_find_num_nodes_before) {}
TEST_F(dllistTests, DISABLED_dllist_node_apply) {}
TEST_F(dllistTests, dllist_init_InitsTheStruct) {
	// Assign
	dllist d;
	memset(&d, 0, sizeof(dllist)); //ensure everything is 0

	//Act
	dllist_init(&d, (dllist_node_allocator)1, (dllist_node_deallocator)2, (dllist_node_initializer)3, (dllist_node_destructor)4, (dllist_node_comparator)5, (dllist_node_copier)6, (void*)7);

	//Assert
	EXPECT_EQ(d.allocate, (dllist_node_allocator)1);
	EXPECT_EQ(d.deallocate, (dllist_node_deallocator)2);
	EXPECT_EQ(d.initialize, (dllist_node_initializer)3);
	EXPECT_EQ(d.destroy, (dllist_node_destructor)4);
	EXPECT_EQ(d.compare, (dllist_node_comparator)5);
	EXPECT_EQ(d.copy, (dllist_node_copier)6);
	EXPECT_EQ(d.user_data, (void*)7);
	EXPECT_EQ(d.head.next, &d.tail);
	EXPECT_EQ(d.tail.previous, &d.head);
	EXPECT_EQ(d.head.previous, (dllist_node*)NULL);
	EXPECT_EQ(d.tail.next, (dllist_node*)NULL);
}

TEST_F(dllistTests, dllist_init_copy_InitsTheStruct) {
	// Assign
	dllist d1, d2;
	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0
	d2.allocate = (dllist_node_allocator)1;
	d2.deallocate = (dllist_node_deallocator)2;
	d2.initialize = (dllist_node_initializer)3;
	d2.destroy = (dllist_node_destructor)4;
	d2.compare = (dllist_node_comparator)5;
	d2.copy = (dllist_node_copier)6;
	d2.user_data = (void*)7;
	//ensure dllist is setted up
	d2.head.next = &d2.tail;
	d2.head.previous = nullptr;
	d2.tail.previous = &d2.head;
	d2.tail.next = nullptr;
	//Act
	dllist_init_copy(&d1, &d2);

	//Assert
	EXPECT_EQ(d1.allocate, d2.allocate);
	EXPECT_EQ(d1.deallocate, d2.deallocate);
	EXPECT_EQ(d1.initialize, d2.initialize);
	EXPECT_EQ(d1.destroy, d2.destroy);
	EXPECT_EQ(d1.compare, d2.compare);
	EXPECT_EQ(d1.copy, d2.copy);
	EXPECT_EQ(d1.user_data, d2.user_data);
	EXPECT_EQ(d1.head.next, &d1.tail);
	EXPECT_EQ(d1.tail.previous, &d1.head);
	EXPECT_EQ(d1.head.previous, (dllist_node*)NULL);
	EXPECT_EQ(d1.tail.next, (dllist_node*)NULL);
}

TEST_F(dllistTests, dllist_init_copy_WithoutAllocatorOrCopierCreatesCountBaseNodes) {
	// Memory Manager setup
	MemoryManagerTN<dllist_node, 10> localMemoryManager;

	// Assign
	dllist d1, d2;
	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0
	d2.allocate = nullptr;
	d2.deallocate = (dllist_node_deallocator)2;
	d2.initialize = (dllist_node_initializer)3;
	d2.destroy = (dllist_node_destructor)4;
	d2.compare = (dllist_node_comparator)5;
	d2.allocate_custom = nullptr;
	d2.copy = nullptr;
	d2.user_data = (void*)7;

	//set up next data
	dllist_node pool[10];
	pool[0].previous = nullptr;
	pool[0].next = &pool[1];
	for (int i = 1; i < 9; i++) {
		pool[i].previous = &pool[i - 1];
		pool[i].next = &pool[i + 1];
	}
	pool[9].previous = &pool[8];
	pool[9].next = nullptr;
	//ensure dllist is setted up
	d2.head.next = &pool[0];
	pool[0].previous = &d2.head;
	d2.head.previous = nullptr;
	d2.tail.previous = &pool[9];
	pool[9].next = &d2.tail;
	d2.tail.next = nullptr;

	//Act
	ASSERT_NO_THROW(dllist_init_copy(&d1, &d2)) << "Memory manager failed";

	//Assert
	EXPECT_EQ(d1.allocate, d2.allocate);
	EXPECT_EQ(d1.deallocate, d2.deallocate);
	EXPECT_EQ(d1.initialize, d2.initialize);
	EXPECT_EQ(d1.destroy, d2.destroy);
	EXPECT_EQ(d1.compare, d2.compare);
	EXPECT_EQ(d1.copy, d2.copy);
	EXPECT_EQ(d1.user_data, d2.user_data);

	dllist_node *current_top = d1.head.next, *current_bottom = d1.tail.previous;
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(current_top) << "chain broke from head to bottom at " << i;
		ASSERT_TRUE(current_top->next);
		ASSERT_TRUE(current_bottom) << "chain broke from bottom to head at " << (10 - i);
		ASSERT_TRUE(current_bottom->previous);
		current_top = current_top->next;
		current_bottom = current_bottom->previous;
	}
	EXPECT_EQ(current_top, &d1.tail) << "chain broke at bottom";
	EXPECT_EQ(current_bottom, &d1.head) << "chain broke at head";
}

TEST_F(dllistTests, dllist_init_copy_WithAllocatorWithoutCopierCreatesCountCustomUninitializedNodes) {
	// Memory Manager setup
	MemoryManager0 localMemoryManager; // prohibid allocations from the system. custom allocations will work

	// Assign
	dllist d1, d2;
	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0

	struct AllocationData {
		dllist_node_long allocatedPool[10];
		size_t allocated_index;
	} allocData;
	memset(&allocData, 0, sizeof(allocData));
	for (long i = 10; i > 0; i--)
		allocData.allocatedPool[10 - i].value = i - 1L;

	d2.allocate = (dllist_node_allocator)[](void* user_data) {
		dllist_node_long* allocationPool = ((AllocationData*)user_data)->allocatedPool;
		if (((AllocationData*)user_data)->allocated_index <= 10)
			return (dllist_node*)&allocationPool[((AllocationData*)user_data)->allocated_index++];
		else
			throw std::exception("Requested more memory than expected");
	};
	d2.deallocate = (dllist_node_deallocator)2;
	d2.initialize = (dllist_node_initializer)3;
	d2.destroy = (dllist_node_destructor)4;
	d2.compare = (dllist_node_comparator)5;
	d2.copy = nullptr;
	d2.user_data = &allocData;

	//set up next data
	dllist_node_long pool[10];
	pool[0].value = 0;
	pool[0].node.previous = nullptr;
	pool[0].node.next = (dllist_node*)&pool[1];
	for (int i = 1; i < 9; i++) {
		pool[i].value = i;
		pool[i].node.previous = (dllist_node*)&pool[i - 1];
		pool[i].node.next = (dllist_node*)&pool[i + 1];
	}
	pool[9].value = 9;
	pool[9].node.previous = (dllist_node*)&pool[8];
	pool[9].node.next = nullptr;
	//ensure dllist is setted up
	d2.head.next = (dllist_node*)&pool[0];
	pool[0].node.previous = &d2.head;
	d2.head.previous = nullptr;
	d2.tail.previous = (dllist_node*)&pool[9];
	pool[9].node.next = &d2.tail;
	d2.tail.next = nullptr;

	//Act
	EXPECT_NO_THROW(dllist_init_copy(&d1, &d2)) << "Memory manager failed";

	//Assert
	EXPECT_EQ(d1.allocate, d2.allocate);
	EXPECT_EQ(d1.deallocate, d2.deallocate);
	EXPECT_EQ(d1.initialize, d2.initialize);
	EXPECT_EQ(d1.destroy, d2.destroy);
	EXPECT_EQ(d1.compare, d2.compare);
	EXPECT_EQ(d1.copy, d2.copy);
	EXPECT_EQ(d1.user_data, d2.user_data);

	dllist_node *current_top = d1.head.next, *current_bottom = d1.tail.previous;
	for (int i = 0; i < 10; i++) {
		EXPECT_TRUE(current_top) << "chain broke from head to bottom at " << i;
		EXPECT_TRUE(current_bottom) << "chain broke from bottom to head at " << (10 - i);
		EXPECT_EQ(((dllist_node_long*)current_top)->value, allocData.allocatedPool[i].value) << "values at " << i << " do not match!";
		current_top = current_top->next;
		current_bottom = current_bottom->previous;
	}
	EXPECT_EQ(current_top, &d1.tail) << "chain broke at bottom";
	EXPECT_EQ(current_bottom, &d1.head) << "chain broke at head";
}

TEST_F(dllistTests, dllist_init_copy_WithAllocatorAndCopierCopiesCountCustomUninitializedNodes) {
	// Memory Manager setup
	MemoryManager0 localMemoryManager; // prohibid allocations from the system. custom allocations will work

									   // Assign
	dllist d1, d2;
	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0

	struct AllocationData {
		dllist_node_long allocatedPool[10];
		size_t allocated_index;
	} allocData;
	memset(&allocData, 0, sizeof(allocData));
	for (long i = 10; i > 0; i--)
		allocData.allocatedPool[10 - i].value = i - 1L;

	d2.allocate = (dllist_node_allocator)[]( void* user_data) {
		dllist_node_long* allocationPool = ((AllocationData*)user_data)->allocatedPool;
		if (((AllocationData*)user_data)->allocated_index <= 10)
			return (dllist_node*)&allocationPool[((AllocationData*)user_data)->allocated_index++];
		else
			throw std::exception("Requested more memory than expected");
	};
	d2.deallocate = (dllist_node_deallocator)2;
	d2.initialize = (dllist_node_initializer)3;
	d2.destroy = (dllist_node_destructor)4;
	d2.compare = (dllist_node_comparator)5;
	d2.copy = (dllist_node_copier)[](dllist_node* dst, dllist_node* src, void* user_data) {
		((dllist_node_long*)dst)->value = ((dllist_node_long*)src)->value;
	};
	d2.user_data = &allocData;

	//set up next data
	dllist_node_long pool[10];
	pool[0].value = 0;
	pool[0].node.previous = nullptr;
	pool[0].node.next = (dllist_node*)&pool[1];
	for (int i = 1; i < 9; i++) {
		pool[i].value = i;
		pool[i].node.previous = (dllist_node*)&pool[i - 1];
		pool[i].node.next = (dllist_node*)&pool[i + 1];
	}
	pool[9].value = 9;
	pool[9].node.previous = (dllist_node*)&pool[8];
	pool[9].node.next = nullptr;
	//ensure dllist is setted up
	d2.head.next = (dllist_node*)&pool[0];
	pool[0].node.previous = &d2.head;
	d2.head.previous = nullptr;
	d2.tail.previous = (dllist_node*)&pool[9];
	pool[9].node.next = &d2.tail;
	d2.tail.next = nullptr;

	//Act
	EXPECT_NO_THROW(dllist_init_copy(&d1, &d2)) << "Memory manager failed";

	//Assert
	EXPECT_EQ(d1.allocate, d2.allocate);
	EXPECT_EQ(d1.deallocate, d2.deallocate);
	EXPECT_EQ(d1.initialize, d2.initialize);
	EXPECT_EQ(d1.destroy, d2.destroy);
	EXPECT_EQ(d1.compare, d2.compare);
	EXPECT_EQ(d1.copy, d2.copy);
	EXPECT_EQ(d1.user_data, d2.user_data);

	dllist_node *current_top = d1.head.next, *current_bottom = d1.tail.previous;
	for (int i = 0; i < 10; i++) {
		EXPECT_TRUE(current_top) << "chain broke from head to bottom at " << i;
		EXPECT_TRUE(current_bottom) << "chain broke from bottom to head at " << (10 - i);
		EXPECT_EQ(((dllist_node_long*)current_top)->value, allocData.allocatedPool[i].value) << "values at " << i << " do not match!";
		current_top = current_top->next;
		current_bottom = current_bottom->previous;
	}
	EXPECT_EQ(current_top, &d1.tail) << "chain broke at bottom";
	EXPECT_EQ(current_bottom, &d1.head) << "chain broke at head";
}
TEST_F(dllistTests, dllist_init_move_MovesTheData) {
	dllist d1, d2;
	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0

	memset(&d1, 0, sizeof(dllist)); //ensure everything is 0
	d2.allocate = (dllist_node_allocator)1;
	d2.deallocate = (dllist_node_deallocator)2;
	d2.initialize = (dllist_node_initializer)3;
	d2.destroy = (dllist_node_destructor)4;
	d2.compare = (dllist_node_comparator)5;
	d2.copy = (dllist_node_copier)6;
	d2.user_data = (void*)7;

	//set up next data
	//set up next data
	dllist_node_long pool[10];
	pool[0].value = 0;
	pool[0].node.previous = nullptr;
	pool[0].node.next = (dllist_node*)&pool[1];
	for (int i = 1; i < 9; i++) {
		pool[i].value = i;
		pool[i].node.previous = (dllist_node*)&pool[i - 1];
		pool[i].node.next = (dllist_node*)&pool[i + 1];
	}
	pool[9].value = 9;
	pool[9].node.previous = (dllist_node*)&pool[8];
	pool[9].node.next = nullptr;
	//ensure dllist is setted up
	d2.head.next = (dllist_node*)&pool[0];
	pool[0].node.previous = &d2.head;
	d2.head.previous = nullptr;
	d2.tail.previous = (dllist_node*)&pool[9];
	pool[9].node.next = &d2.tail;
	d2.tail.next = nullptr;

	//Act
	dllist_init_move(&d1, &d2);

	//Assert
	EXPECT_EQ(d1.allocate, (dllist_node_allocator)1);
	EXPECT_EQ(d1.deallocate, (dllist_node_deallocator)2);
	EXPECT_EQ(d1.initialize, (dllist_node_initializer)3);
	EXPECT_EQ(d1.destroy, (dllist_node_destructor)4);
	EXPECT_EQ(d1.compare, (dllist_node_comparator)5);
	EXPECT_EQ(d1.copy, (dllist_node_copier)6);
	EXPECT_EQ(d1.user_data, (void*)7);

	EXPECT_EQ(d2.allocate, (dllist_node_allocator)1);
	EXPECT_EQ(d2.deallocate, (dllist_node_deallocator)2);
	EXPECT_EQ(d2.initialize, (dllist_node_initializer)3);
	EXPECT_EQ(d2.destroy, (dllist_node_destructor)4);
	EXPECT_EQ(d2.compare, (dllist_node_comparator)5);
	EXPECT_EQ(d2.copy, (dllist_node_copier)6);
	EXPECT_EQ(d2.user_data, (void*)7);

	EXPECT_EQ(d1.head.next, (dllist_node*)&pool[0]);
	EXPECT_EQ(d1.tail.previous, (dllist_node*)&pool[9]);
}
TEST_F(dllistTests, DISABLED_dllist_init_copy){}
TEST_F(dllistTests, DISABLED_dllist_init_move){}
TEST_F(dllistTests, DISABLED_dllist_destroy){}
TEST_F(dllistTests, DISABLED_dllist_apply){}
TEST_F(dllistTests, DISABLED_dllist_apply_all){}
TEST_F(dllistTests, DISABLED_dllist_last){}
TEST_F(dllistTests, DISABLED_dllist_first){}
TEST_F(dllistTests, DISABLED_dllist_end){}
TEST_F(dllistTests, DISABLED_dllist_begin){}
TEST_F(dllistTests, DISABLED_dllist_empty){}
TEST_F(dllistTests, DISABLED_dllist_size){}
TEST_F(dllistTests, DISABLED_dllist_peek_top){}
TEST_F(dllistTests, DISABLED_dllist_peek_bottom){}
TEST_F(dllistTests, DISABLED_dllist_push_front){}
TEST_F(dllistTests, DISABLED_dllist_push_back){}
TEST_F(dllistTests, DISABLED_dllist_push_front_custom){}
TEST_F(dllistTests, DISABLED_dllist_push_back_custom){}
TEST_F(dllistTests, DISABLED_dllist_pop_front){}
TEST_F(dllistTests, DISABLED_dllist_pop_back){}
TEST_F(dllistTests, DISABLED_dllist_get){}
TEST_F(dllistTests, DISABLED_dllist_insert){}
TEST_F(dllistTests, DISABLED_dllist_insert_sorted){}
TEST_F(dllistTests, DISABLED_dllist_remove){}
TEST_F(dllistTests, DISABLED_dllist_remove_custom){}
TEST_F(dllistTests, DISABLED_dllist_remove_times){}
TEST_F(dllistTests, DISABLED_dllist_remove_times_custom){}
TEST_F(dllistTests, DISABLED_dllist_remove_at){}
TEST_F(dllistTests, DISABLED_dllist_sort){}
TEST_F(dllistTests, DISABLED_dllist_clear){}
TEST_F(dllistTests, DISABLED_dllist_node_void_ptr_allocate){}
TEST_F(dllistTests, DISABLED_dllist_node_void_ptr_initializer){}
TEST_F(dllistTests, DISABLED_dllist_node_void_ptr_deallocate){}
TEST_F(dllistTests, DISABLED_dllist_node_void_ptr_comparator){}
TEST_F(dllistTests, DISABLED_dllist_init_void_ptr){}
TEST_F(dllistTests, DISABLED_dllist_node_long_allocate){}
TEST_F(dllistTests, DISABLED_dllist_node_long_initializer){}
TEST_F(dllistTests, DISABLED_dllist_node_long_deallocate){}
TEST_F(dllistTests, DISABLED_dllist_node_long_comparator){}
TEST_F(dllistTests, DISABLED_dllist_init_long){}
TEST_F(dllistTests, DISABLED_dllist_node_float_allocate){}
TEST_F(dllistTests, DISABLED_dllist_node_float_initializer){}
TEST_F(dllistTests, DISABLED_dllist_node_float_deallocate){}
TEST_F(dllistTests, DISABLED_dllist_node_float_comparator){}