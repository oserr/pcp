#include <sstream>

#include "gtest/gtest.h"

#include "lockfree_list.h"

namespace {

TEST(LockFreeList, DefaultCtorInitalizesListCorrectly) {
  LockFreeList<int> lf;

  EXPECT_EQ(0u, lf.size);
  EXPECT_EQ(lf.tail, lf.head->next);
}

TEST(LockFreeList, InsertWorksCorrectly) {
  LockFreeList<int> lf;

  lf.Insert(1);
  EXPECT_EQ(1, lf.head->next->value);
  lf.Insert(2);
  EXPECT_EQ(2, lf.head->next->next->value);
}

TEST(LockFreeList, InsertUniqueWorksCorrectly) {
  LockFreeList<int> lf;
  
  EXPECT_TRUE(lf.Insert(1));
  EXPECT_FALSE(lf.InsertUnique(1));
  EXPECT_TRUE(lf.InsertUnique(2));
  EXPECT_EQ(2, lf.head->next->next->value);
  EXPECT_FALSE(lf.InsertUnique(2));
  EXPECT_TRUE(lf.InsertUnique(3));
  EXPECT_FALSE(lf.InsertUnique(3));
}

// TEST(LockFreeList, RemoveCanRemoveWhenListOnlyHasOne) {
//   lf.Insert(1);
//   EXPECT_EQ(1, lf.head->value);
//   EXPECT_TRUE(lf.Remove(1));
//   EXPECT_EQ(nullptr, lf.head);
// }

// TEST(LockFreeList, RemoveCanRemoveFirstOfMany) {
//   lf.Insert(1);
//   lf.Insert(2);
//   lf.Insert(3);
//   EXPECT_TRUE(lf.Remove(3));
//   EXPECT_EQ(2, lf.head->value);
//   EXPECT_EQ(1, lf.head->next->value);
// }

// TEST(LockFreeList, RemoveCanRemoveMiddleOfMany) {
//   lf.Insert(1);
//   lf.Insert(2);
//   lf.Insert(3);
//   EXPECT_TRUE(lf.Remove(2));
//   EXPECT_EQ(3, lf.head->value);
//   EXPECT_EQ(1, lf.head->next->value);
//   EXPECT_EQ(lf.head, lf.head->next->prev);
// }

} // namespace
