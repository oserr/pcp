#include <sstream>

#include "gtest/gtest.h"

#include "dlnode.h"

namespace {

TEST(DlNode, DefaultCtorInitalizesNodeCorrectly) {
  DlNode<int> node;

  EXPECT_EQ(0, node.value);
  EXPECT_EQ(nullptr, node.prev);
  EXPECT_EQ(nullptr, node.next);
}

TEST(DlNode, ValueCtorInitalizesNodeCorrectly) {
  DlNode<int> node(4);

  EXPECT_EQ(4, node.value);
  EXPECT_EQ(nullptr, node.prev);
  EXPECT_EQ(nullptr, node.next);
}

TEST(DlNode, AllParamCtorInitializesNodeCorrectly) {
  DlNode<int> nodePrev(1);
  DlNode<int> nodeNext(2);
  DlNode<int> node(3, &nodePrev, &nodeNext);

  EXPECT_EQ(3, node.value);
  EXPECT_EQ(&nodePrev, node.prev);
  EXPECT_EQ(&nodeNext, node.next);
}

TEST(DlNode, OutputOpWordsCorrectly) {
  DlNode<int> nodePrev(1);
  DlNode<int> nodeNext(2);
  DlNode<int> node(3, &nodePrev, &nodeNext);

  std::ostringstream oss;
  oss << node;

  std::ostringstream ossExpected;
  ossExpected << "DlNode(" << node.value << ',' << node.prev << ',' << node.next
              << ')';

  EXPECT_EQ(3, node.value);
  EXPECT_EQ(ossExpected.str(), oss.str());
}

} // namespace
