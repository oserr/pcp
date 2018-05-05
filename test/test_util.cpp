#include <string>

#include "gtest/gtest.h"

#include "util.h"

namespace {

TEST(Util, SplitReturnsEmptyVectorFromEmptyString) {
  auto result = split("", ',');
  EXPECT_TRUE(result.empty());
}

TEST(Util, SplitReturnsOneWordWhenStringContainOnlyOneItem) {
  std::string word("word");
  auto result = split(word, ',');
  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(word, result[0]);

  result = split("word,", ',');
  ASSERT_EQ(1u, result.size());
  EXPECT_EQ(word, result[0]);
}

TEST(Util, SplitReturnsMultipleWordsCorrectly) {
  auto result = split("word1,word2", ',');
  ASSERT_EQ(2u, result.size());
  EXPECT_EQ("word1", result[0]);
  EXPECT_EQ("word2", result[1]);
}

TEST(Util, ToLowerConvertsToLowerCaseCorrectly) {
  auto result = toLower("HELLO");
  EXPECT_EQ("hello", result);
  result = toLower("HellO");
  EXPECT_EQ("hello", result);
}

} // namespace
