#include <algorithm>
#include <functional>
#include <random>
#include <thread>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

#include "coarse_grain_list.h"
#include "fine_grain_list.h"
#include "lockfree_list.h"
#include "nonblocking_list.h"

namespace {

constexpr int kNumThreads = 4;
// At 10000 the tests start to take a little longer to run.
constexpr int kIntsPerThread = 10000;

template <typename TIntList> struct IntAsyncListTest : public ::testing::Test {
  using ListType = TIntList;
  TIntList intList{};
  TIntList otherIntList{};
  std::vector<std::vector<int>> numberLists{};

  /**
   * Setups the array of numbers each thread will use to insert and remove items
   * from intList.
   */
  void SetUp() override {
    for (int t = 0; t < kNumThreads; ++t) {
      std::vector<int> numbers(kIntsPerThread);
      auto iter = numbers.begin();
      for (int i = t * kIntsPerThread, last = i + kIntsPerThread; i < last; ++i)
        *iter++ = i;
      numberLists.emplace_back(std::move(numbers));
    }
  }

  /**
   * Inserts numbers into intList. It is meant to run in its own thread.
   * @param index The index associated with the thread, used to access its
   *  corresponding list of numbers.
   * @details The numbers are shuffled before any of them are inserted into the
   *  list.
   */
  void DoInsert(int index) {
    auto &numbers = numberLists[index];
    std::shuffle(numbers.begin(), numbers.end(),
                 std::default_random_engine(index * 33));
    for (auto n : numbers)
      EXPECT_TRUE(intList.Insert(n));
  }

  /**
   * Removes numbers from intList. It is meant to run in its own thread.
   * @param index The index associated with the thread, used to access its
   *  corresponding list of numbers.
   * @details The numbers are shuffled before any of them are removed from the
   *  list.
   */
  void DoRemove(int index) {
    auto &numbers = numberLists[index];
    std::shuffle(numbers.begin(), numbers.end(),
                 std::default_random_engine(index * 71));
    for (auto n : numbers)
      EXPECT_TRUE(intList.Remove(n));
  }
};

// Google test needs this declaration before the type-parameterized tests are
// created
TYPED_TEST_CASE_P(IntAsyncListTest);

TYPED_TEST_P(IntAsyncListTest, CanInsertAndRemoveItemsWithoutDeadlock) {
  std::thread threads[kNumThreads];

  EXPECT_TRUE(this->intList.Empty());

  for (int i = 0; i < kNumThreads; ++i)
    threads[i] = std::thread(&IntAsyncListTest<TypeParam>::DoInsert, this, i);

  std::for_each(threads, threads + kNumThreads,
                std::mem_fn(&std::thread::join));

  constexpr unsigned totalInts = kNumThreads * kIntsPerThread;
  EXPECT_EQ(totalInts, this->intList.Size());

  for (int i = 0; i < kNumThreads; ++i)
    threads[i] = std::thread(&IntAsyncListTest<TypeParam>::DoRemove, this, i);

  std::for_each(threads, threads + kNumThreads,
                std::mem_fn(&std::thread::join));
  EXPECT_TRUE(this->intList.Empty());
}

REGISTER_TYPED_TEST_CASE_P(IntAsyncListTest,
                           CanInsertAndRemoveItemsWithoutDeadlock);
INSTANTIATE_TYPED_TEST_CASE_P(CoarseGrainList, IntAsyncListTest,
                              CoarseGrainList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(FineGrainList, IntAsyncListTest,
                              FineGrainList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(LockFreeList, IntAsyncListTest,
                              LockFreeList<int>);
INSTANTIATE_TYPED_TEST_CASE_P(NonBlockingList, IntAsyncListTest,
                              NonBlockingList<int>);

} // namespace
