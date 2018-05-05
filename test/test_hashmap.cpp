#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "coarse_grain_list.h"
#include "dllist.h"
#include "fine_grain_list.h"
#include "hashmap.h"
#include "libcuckoo_hashmap.h"
#include "lockfree_list.h"
#include "nonblocking_list.h"
#include "tbb_hashmap.h"

namespace {

template <typename TStringHashMap>
struct StringHashMapTest : public ::testing::Test {
  TStringHashMap hm{};
};

// Google test needs this declaration before the type-parameterized tests are
// created
TYPED_TEST_CASE_P(StringHashMapTest);

TYPED_TEST_P(StringHashMapTest, SizeWorksCorrectly) {
  this->hm.Insert("color", "blue");
  this->hm.Insert("hair", "brown");

  EXPECT_EQ(2u, this->hm.Size());
}

TYPED_TEST_P(StringHashMapTest, HasWorksCorrectly) {
  this->hm.Insert("color", "blue");
  this->hm.Insert("hair", "brown");

  EXPECT_TRUE(this->hm.Has("color"));
  EXPECT_FALSE(this->hm.Has("size"));
}

TYPED_TEST_P(StringHashMapTest, InsertGetWorksCorrectly) {
  this->hm.Insert("color", "blue");
  this->hm.Insert("hair", "brown");
  this->hm.Insert("size", "small");

  EXPECT_EQ("blue", this->hm["color"]);
  EXPECT_EQ("brown", this->hm["hair"]);
  EXPECT_EQ("small", this->hm["size"]);
  EXPECT_EQ(3u, this->hm.Size());

  EXPECT_EQ("", this->hm["nothere"]);
  EXPECT_EQ(4u, this->hm.Size());
}

TYPED_TEST_P(StringHashMapTest, GetsNonExistingWorksCorrectly) {
  this->hm.Insert("color", "blue");
  EXPECT_EQ(1u, this->hm.Size());
  EXPECT_EQ("", this->hm["size"]);
  EXPECT_EQ(2u, this->hm.Size());
}

TYPED_TEST_P(StringHashMapTest, ReadWithSubscriptWorksCorrectly) {
  this->hm.Insert("color", "blue");
  this->hm.Insert("hair", "brown");

  EXPECT_EQ("brown", this->hm["hair"]);
  EXPECT_EQ("blue", this->hm["color"]);
  EXPECT_EQ(2u, this->hm.Size());
}

TYPED_TEST_P(StringHashMapTest, RemoveWorksCorrectly) {
  this->hm.Insert("color", "blue");
  this->hm.Insert("hair", "brown");
  EXPECT_TRUE(this->hm.Has("color"));
  EXPECT_EQ(2u, this->hm.Size());

  this->hm.Remove("color");
  EXPECT_FALSE(this->hm.Has("color"));
  EXPECT_EQ(1u, this->hm.Size());

  this->hm.Remove("nothere");
  EXPECT_EQ(1u, this->hm.Size());
}

using DlListStringHashMap = HashMap<std::string, std::string, DlList>;
using CoarseGrainListStringHashMap =
    HashMap<std::string, std::string, CoarseGrainList>;
using FineGrainListStringHashMap =
    HashMap<std::string, std::string, FineGrainList>;
using NonBlockingListStringHashMap =
    HashMap<std::string, std::string, NonBlockingList>;
using LockFreeListStringHashMap =
    HashMap<std::string, std::string, LockFreeList>;
using LibCuckooStringHashMap = LibCuckooHashMap<std::string, std::string>;
using TbbStringHashMap = TbbHashMap<std::string, std::string>;

REGISTER_TYPED_TEST_CASE_P(StringHashMapTest, SizeWorksCorrectly,
                           HasWorksCorrectly, InsertGetWorksCorrectly,
                           GetsNonExistingWorksCorrectly,
                           ReadWithSubscriptWorksCorrectly,
                           RemoveWorksCorrectly);

INSTANTIATE_TYPED_TEST_CASE_P(DlList, StringHashMapTest, DlListStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(CoarseGrainList, StringHashMapTest,
                              CoarseGrainListStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(FineGrainList, StringHashMapTest,
                              FineGrainListStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(NonBlockingList, StringHashMapTest,
                              NonBlockingListStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(LockFreeList, StringHashMapTest,
                              LockFreeListStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(LibCuckooHashMap, StringHashMapTest,
                              LibCuckooStringHashMap);
INSTANTIATE_TYPED_TEST_CASE_P(TbbHashMap, StringHashMapTest, TbbStringHashMap);

} // namespace
