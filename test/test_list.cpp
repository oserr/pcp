#include <sstream>

#include "gtest/gtest.h"

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "nonblocking_list.h"

namespace {

template <typename TIntList> struct IntListTest : public ::testing::Test {
  TIntList intList{};
  TIntList otherIntList{};
};

// Google test needs this declaration before the type-parameterized tests are
// created
TYPED_TEST_CASE_P(IntListTest);

TYPED_TEST_P(IntListTest, DefaultCtorInitalizesListCorrectly) {
  EXPECT_EQ(0u, this->intList.size);
  EXPECT_EQ(nullptr, this->intList.head);
}

TYPED_TEST_P(IntListTest, InsertWorksCorrectly) {
  this->intList.Insert(1);
  EXPECT_EQ(1, this->intList.head->value);

  this->intList.Insert(2);
  EXPECT_EQ(2, this->intList.head->value);
  EXPECT_EQ(nullptr, this->intList.head->prev);
  EXPECT_EQ(1, this->intList.head->next->value);
  EXPECT_EQ(this->intList.head, this->intList.head->next->prev);
}

TYPED_TEST_P(IntListTest, InsertUniqueWorksCorrectly) {
  EXPECT_NE(nullptr, this->intList.Insert(1));
  EXPECT_FALSE(this->intList.InsertUnique(1));
  EXPECT_TRUE(this->intList.InsertUnique(2));
  EXPECT_EQ(2, this->intList.head->next->value);
  EXPECT_EQ(this->intList.head, this->intList.head->next->prev);
  EXPECT_FALSE(this->intList.InsertUnique(2));
  EXPECT_TRUE(this->intList.InsertUnique(3));
  EXPECT_EQ(3, this->intList.head->next->next->value);
  EXPECT_FALSE(this->intList.InsertUnique(3));
}

TYPED_TEST_P(IntListTest, RemoveCanRemoveWhenListOnlyHasOne) {
  this->intList.Insert(1);
  EXPECT_EQ(1, this->intList.head->value);
  EXPECT_TRUE(this->intList.Remove(1));
  EXPECT_EQ(nullptr, this->intList.head);
}

TYPED_TEST_P(IntListTest, RemoveCanRemoveFirstOfMany) {
  this->intList.Insert(1);
  this->intList.Insert(2);
  this->intList.Insert(3);
  EXPECT_TRUE(this->intList.Remove(3));
  EXPECT_EQ(2, this->intList.head->value);
  EXPECT_EQ(1, this->intList.head->next->value);
}

TYPED_TEST_P(IntListTest, RemoveCanRemoveMiddleOfMany) {
  this->intList.Insert(1);
  this->intList.Insert(2);
  this->intList.Insert(3);
  EXPECT_TRUE(this->intList.Remove(2));
  EXPECT_EQ(3, this->intList.head->value);
  EXPECT_EQ(1, this->intList.head->next->value);
  EXPECT_EQ(this->intList.head, this->intList.head->next->prev);
}

TYPED_TEST_P(IntListTest, RemoveCanRemoveLastOfMany) {
  this->intList.Insert(1);
  this->intList.Insert(2);
  this->intList.Insert(3);
  EXPECT_TRUE(this->intList.Remove(1));
  EXPECT_EQ(3, this->intList.head->value);
  EXPECT_EQ(2, this->intList.head->next->value);
  EXPECT_EQ(this->intList.head, this->intList.head->next->prev);
}

TYPED_TEST_P(IntListTest, ContainsWorksCorrectly) {
  EXPECT_FALSE(this->intList.Contains(100));
  this->intList.Insert(5);
  this->intList.Insert(10);
  this->intList.Insert(11);
  this->intList.Insert(33);
  EXPECT_TRUE(this->intList.Contains(5));
  EXPECT_TRUE(this->intList.Contains(11));
  this->intList.Remove(11);
  EXPECT_FALSE(this->intList.Contains(11));
}

TYPED_TEST_P(IntListTest, FindWorksCorrectly) {
  int numbers[] = {5, 10, 11, 33};
  for (auto num : numbers)
    this->intList.Insert(num);

  for (auto num : numbers) {
    auto ptr = this->intList.Find(num);
    EXPECT_TRUE(ptr != nullptr and num == *ptr);
  }

  int otherNum[] = {100, 200, 300, 400};
  for (auto num : otherNum) {
    auto ptr = this->intList.Find(num);
    EXPECT_EQ(nullptr, ptr);
  }
}

TYPED_TEST_P(IntListTest, SizeWorksCorrectly) {
  EXPECT_EQ(0u, this->intList.Size());
  this->intList.Insert(5);
  EXPECT_EQ(1u, this->intList.Size());
  this->intList.Insert(10);
  EXPECT_EQ(2u, this->intList.Size());
  this->intList.Insert(11);
  EXPECT_EQ(3u, this->intList.Size());
  this->intList.Remove(11);
  EXPECT_EQ(2u, this->intList.Size());
}

TYPED_TEST_P(IntListTest, EmptyWorksCorrectly) {
  EXPECT_TRUE(this->intList.Empty());
  this->intList.Insert(5);
  EXPECT_FALSE(this->intList.Empty());
  this->intList.Remove(5);
  EXPECT_TRUE(this->intList.Empty());
}

TYPED_TEST_P(IntListTest, OutputOpWorksCorrectly) {
  std::ostringstream oss, oss1, oss2;
  oss << this->intList;
  EXPECT_EQ("()", oss.str());

  this->intList.Insert(1);
  oss1 << this->intList;
  EXPECT_EQ("(1)", oss1.str());

  this->intList.Insert(2);
  this->intList.Insert(6);
  oss2 << this->intList;
  EXPECT_EQ("(6,2,1)", oss2.str());
}

TYPED_TEST_P(IntListTest, EqualityOpWorksCorrectly) {
  EXPECT_EQ(this->intList, this->otherIntList);

  this->intList.Insert(2);
  EXPECT_NE(this->intList, this->otherIntList);

  this->intList.Insert(3);
  this->intList.Insert(4);
  this->intList.Insert(5);

  this->otherIntList.Insert(2);
  this->otherIntList.Insert(3);
  this->otherIntList.Insert(4);
  this->otherIntList.Insert(5);
  EXPECT_EQ(this->intList, this->otherIntList);

  this->otherIntList.Remove(2);
  EXPECT_NE(this->intList, this->otherIntList);
}

REGISTER_TYPED_TEST_CASE_P(IntListTest, DefaultCtorInitalizesListCorrectly,
                           InsertWorksCorrectly, InsertUniqueWorksCorrectly,
                           RemoveCanRemoveWhenListOnlyHasOne,
                           RemoveCanRemoveFirstOfMany,
                           RemoveCanRemoveMiddleOfMany,
                           RemoveCanRemoveLastOfMany, ContainsWorksCorrectly,
                           FindWorksCorrectly, SizeWorksCorrectly,
                           EmptyWorksCorrectly, OutputOpWorksCorrectly,
                           EqualityOpWorksCorrectly);
INSTANTIATE_TYPED_TEST_CASE_P(DlList, IntListTest, DlList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(CoarseGrainList, IntListTest,
                              CoarseGrainList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(FineGrainList, IntListTest, FineGrainList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(NonBlockingList, IntListTest,
                              NonBlockingList<int>);

} // namespace
