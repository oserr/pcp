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
  EXPECT_EQ(1, (*lf.head->next).value);
  lf.Insert(2);
  EXPECT_EQ(2, (*lf.head->next).value);
  EXPECT_EQ(1, (*(*lf.head->next).next).value);
}

TEST(LockFreeList, InsertUniqueWorksCorrectly) {
  LockFreeList<int> lf;

  EXPECT_TRUE(lf.Insert(1));
  EXPECT_FALSE(lf.InsertUnique(1));
  EXPECT_TRUE(lf.InsertUnique(2));
  EXPECT_EQ(2, (*(*lf.head->next).next).value);
  EXPECT_FALSE(lf.InsertUnique(2));
  EXPECT_TRUE(lf.InsertUnique(3));
  EXPECT_FALSE(lf.InsertUnique(3));
}

TEST(LockFreeList, RemoveCanRemoveWhenListOnlyHasOne) {
  LockFreeList<int> lf;

  lf.Insert(1);
  EXPECT_EQ(1, (*lf.head->next).value);
  EXPECT_TRUE(lf.Remove(1));
  EXPECT_EQ(lf.tail, lf.head->next);
}

TEST(LockFreeList, RemoveCanRemoveFirstOfMany) {
  LockFreeList<int> lf;

  lf.Insert(1);
  lf.Insert(2);
  lf.Insert(3);
  EXPECT_TRUE(lf.Remove(3));
  EXPECT_EQ(2, (*lf.head->next).value);
  EXPECT_EQ(1, (*(*lf.head->next).next).value);
}

TEST(LockFreeList, RemoveCanRemoveMiddleOfMany) {
  LockFreeList<int> lf;

  lf.Insert(1);
  lf.Insert(2);
  lf.Insert(3);
  EXPECT_TRUE(lf.Remove(2));
  EXPECT_EQ(3, (*lf.head->next).value);
  EXPECT_EQ(1, (*(*lf.head->next).next).value);
}

TEST(IntListTest, RemoveCanRemoveLastOfMany) {
  LockFreeList<int> lf;

  lf.Insert(1);
  lf.Insert(2);
  lf.Insert(3);
  EXPECT_TRUE(lf.Remove(1));
  EXPECT_EQ(3, (*lf.head->next).value);
  EXPECT_EQ(2, (*(*lf.head->next).next).value);
}

TEST(IntListTest, ContainsWorksCorrectly) {
  LockFreeList<int> lf;

  EXPECT_FALSE(lf.Contains(100));
  lf.Insert(5);
  lf.Insert(10);
  lf.Insert(11);
  lf.Insert(33);
  EXPECT_TRUE(lf.Contains(5));
  EXPECT_TRUE(lf.Contains(11));
  lf.Remove(11);
  EXPECT_FALSE(lf.Contains(11));
}

TEST(IntListTest, SizeWorksCorrectly) {
  LockFreeList<int> lf;

  EXPECT_EQ(0u, lf.Size());
  lf.Insert(5);
  EXPECT_EQ(1u, lf.Size());
  lf.Insert(10);
  EXPECT_EQ(2u, lf.Size());
  lf.Insert(11);
  EXPECT_EQ(3u, lf.Size());
  lf.Remove(11);
  EXPECT_EQ(2u, lf.Size());
}

TEST(IntListTest, EmptyWorksCorrectly) {
  LockFreeList<int> lf;

  EXPECT_TRUE(lf.Empty());
  lf.Insert(5);
  EXPECT_FALSE(lf.Empty());
  lf.Remove(5);
  EXPECT_TRUE(lf.Empty());
}

TEST(IntListTest, OutputOpWorksCorrectly) {
  LockFreeList<int> lf;

  std::ostringstream oss, oss1, oss2;
  oss << lf;
  EXPECT_EQ("()", oss.str());

  lf.Insert(1);
  oss1 << lf;
  EXPECT_EQ("(1)", oss1.str());

  lf.Insert(2);
  lf.Insert(6);
  oss2 << lf;
  EXPECT_EQ("(6,2,1)", oss2.str());
}

} // namespace
