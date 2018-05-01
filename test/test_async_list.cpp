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
  DlList<int> serialList{};
  TIntList concurrentList{};
  std::vector<std::vector<int>> numberLists{};

  /**
   * Setups the array of numbers each thread will use to insert and remove items
   * from concurrentList.
   */
  void SetUp() override {
    for (int t = 0; t < kNumThreads; ++t) {
      std::vector<int> numbers(kIntsPerThread);
      auto iter = numbers.begin();
      for (int i = t * kIntsPerThread, last = i + kIntsPerThread; i < last; ++i){
        *iter++ = i;
        serialList.Insert(i);
      }
      numberLists.emplace_back(std::move(numbers));
    }
  }

  /**
   * Inserts numbers into concurrentList. It is meant to run in its own thread.
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
      EXPECT_TRUE(concurrentList.Insert(n));
  }

  /**
   * Removes numbers from concurrentList. It is meant to run in its own thread.
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
      EXPECT_TRUE(concurrentList.Remove(n));
  }

  /**
  * Remove all elements from serial list
  */
  void SerialRemoveAll(){
    for (int i = 0; i < kNumThreads; ++i){
      auto &numbers = numberLists[i];
      std::shuffle(numbers.begin(), numbers.end(),
                   std::default_random_engine(i * 33));
      for (auto n : numbers)
        EXPECT_TRUE(serialList.Remove(n)); 
    }    
  }

  /* check if serial and concurrent lists are actually equal */
  bool CompareLists(){
    std::ostringstream oss, osc;
    oss << serialList;
    osc << concurrentList;
    std::stringstream ss(oss.str());
    std::stringstream sc(osc.str());
    std::vector<int> serialV;
    std::vector<int> concurrentV;

    // ignore first (
    ss.seekg(1);  
    sc.seekg(1);

    std::string token;
    int number;
    // convert serialList to vector
    while(std::getline(ss, token, ',')) {
      std::stringstream num(token);
      num >> number;
      serialV.push_back(number);
    }
    std::sort(serialV.begin(), serialV.end());
    // std::cout << "serialVector: ";
    // for (auto const& n : serialV)
    //   std::cout << n << ',';    
    // std::cout << std::endl;

    // convert concurrentList to vector
    while(std::getline(sc, token, ',')) {
      std::stringstream num(token);
      num >> number;
      concurrentV.push_back(number);
    }
    std::sort(concurrentV.begin(), concurrentV.end());
    // std::cout << "concurrentVector: ";
    // for (auto const& n : concurrentV)
    //   std::cout << n << ',';
    // std::cout << std::endl;

    return serialV == concurrentV;
  }
};


// Google test needs this declaration before the type-parameterized tests are
// created
TYPED_TEST_CASE_P(IntAsyncListTest);

TYPED_TEST_P(IntAsyncListTest, CanInsertAndRemoveItemsWithoutDeadlock) {
  std::thread threads[kNumThreads];

  EXPECT_TRUE(this->concurrentList.Empty());
  //std::cout << "InitialList: " << this->serialList << std::endl;

  /* create concurrent list*/
  for (int i = 0; i < kNumThreads; ++i)
    threads[i] = std::thread(&IntAsyncListTest<TypeParam>::DoInsert, this, i);
  
  std::for_each(threads, threads + kNumThreads,
                std::mem_fn(&std::thread::join));

  constexpr unsigned totalInts = kNumThreads * kIntsPerThread;
  EXPECT_EQ(totalInts, this->concurrentList.Size());
  /* check serial and concurrent lists are equal (handles reordering) */
  //EXPECT_TRUE(IntAsyncListTest<TypeParam>::CompareLists());

  /* remove all elements from serial list */
  IntAsyncListTest<TypeParam>::SerialRemoveAll();

  /* remove all elements from concurrent list */
  for (int i = 0; i < kNumThreads; ++i)
    threads[i] = std::thread(&IntAsyncListTest<TypeParam>::DoRemove, this, i);

  std::for_each(threads, threads + kNumThreads,
                std::mem_fn(&std::thread::join));
  
  /* check serial and concurrent lists are equal */
  EXPECT_TRUE(this->concurrentList.Empty());
  EXPECT_TRUE(IntAsyncListTest<TypeParam>::CompareLists());  
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
