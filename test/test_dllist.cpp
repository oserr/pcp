#include <sstream>

#include "gtest/gtest.h"

#include "dllist.h"

namespace {

TEST(DlList, DefaultCtorInitalizesListCorrectly) {
  DlList<int> dlList;

  EXPECT_EQ(0u, dlList.size);
  EXPECT_EQ(nullptr, dlList.head);
}

TEST(DlList, InsertWorksCorrectly) {
  DlList<int> dlList;

  dlList.Insert(1);
  EXPECT_EQ(1, dlList.head->value);

  dlList.Insert(2);
  EXPECT_EQ(2, dlList.head->value);
  EXPECT_EQ(nullptr, dlList.head->prev);
  EXPECT_EQ(1, dlList.head->next->value);
  EXPECT_EQ(dlList.head, dlList.head->next->prev);
}

TEST(DlList, RemoveCanRemoveWhenListOnlyHasOne) {
  DlList<int> dlList;

  dlList.Insert(1);
  EXPECT_EQ(1, dlList.head->value);
  EXPECT_TRUE(dlList.Remove(1));
  EXPECT_EQ(nullptr, dlList.head);
}

TEST(DlList, RemoveCanRemoveFirstOfMany) {
  DlList<int> dlList;

  dlList.Insert(1);
  dlList.Insert(2);
  dlList.Insert(3);
  EXPECT_TRUE(dlList.Remove(3));
  EXPECT_EQ(2, dlList.head->value);
  EXPECT_EQ(1, dlList.head->next->value);
}

TEST(DlList, RemoveCanRemoveMiddleOfMany) {
  DlList<int> dlList;

  dlList.Insert(1);
  dlList.Insert(2);
  dlList.Insert(3);
  EXPECT_TRUE(dlList.Remove(2));
  EXPECT_EQ(3, dlList.head->value);
  EXPECT_EQ(1, dlList.head->next->value);
  EXPECT_EQ(dlList.head, dlList.head->next->prev);
}

TEST(DlList, RemoveCanRemoveLastOfMany) {
  DlList<int> dlList;

  dlList.Insert(1);
  dlList.Insert(2);
  dlList.Insert(3);
  EXPECT_TRUE(dlList.Remove(1));
  EXPECT_EQ(3, dlList.head->value);
  EXPECT_EQ(2, dlList.head->next->value);
  EXPECT_EQ(dlList.head, dlList.head->next->prev);
}

TEST(DlList, ContainsWorksCorrectly) {
  DlList<int> dlList;

  EXPECT_FALSE(dlList.Contains(100));
  dlList.Insert(5);
  dlList.Insert(10);
  dlList.Insert(11);
  dlList.Insert(33);
  EXPECT_TRUE(dlList.Contains(5));
  EXPECT_TRUE(dlList.Contains(11));
  dlList.Remove(11);
  EXPECT_FALSE(dlList.Contains(11));
}

TEST(DlList, SizeWorksCorrectly) {
  DlList<int> dlList;

  EXPECT_EQ(0u, dlList.Size());
  dlList.Insert(5);
  EXPECT_EQ(1u, dlList.Size());
  dlList.Insert(10);
  EXPECT_EQ(2u, dlList.Size());
  dlList.Insert(11);
  EXPECT_EQ(3u, dlList.Size());
  dlList.Remove(11);
  EXPECT_EQ(2u, dlList.Size());
}

TEST(DlList, EmptyWorksCorrectly) {
  DlList<int> dlList;

  EXPECT_TRUE(dlList.Empty());
  dlList.Insert(5);
  EXPECT_FALSE(dlList.Empty());
  dlList.Remove(5);
  EXPECT_TRUE(dlList.Empty());
}

TEST(DlNode, OutputOpWordsCorrectly) {
  DlList<int> dlList;
  std::ostringstream oss, oss1, oss2;

  oss << dlList;
  EXPECT_EQ("DlList()", oss.str());

  dlList.Insert(1);
  oss1 << dlList;
  EXPECT_EQ("DlList(1)", oss1.str());

  dlList.Insert(2);
  dlList.Insert(6);
  oss2 << dlList;
  EXPECT_EQ("DlList(6,2,1)", oss2.str());
}

TEST(DlNode, EqualityOpWordsCorrectly) {
  DlList<int> dlList1, dlList2;

  EXPECT_EQ(dlList1, dlList2);

  dlList1.Insert(2);
  EXPECT_NE(dlList1, dlList2);

  dlList1.Insert(3);
  dlList1.Insert(4);
  dlList1.Insert(5);

  dlList2.Insert(2);
  dlList2.Insert(3);
  dlList2.Insert(4);
  dlList2.Insert(5);
  EXPECT_EQ(dlList1, dlList2);

  dlList2.Remove(2);
  EXPECT_NE(dlList1, dlList2);
}

} // namespace
