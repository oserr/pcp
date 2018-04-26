#include <sstream>

#include "gtest/gtest.h"

#include "hashmap.h"
#include <string>

namespace {

template <typename K, typename V> using DlListHashMap = HashMap<K, V, DlList>;

TEST(HashMap, SizeWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  hm.Insert("hair", "brown");

  EXPECT_EQ(2u, hm.Size());
}

TEST(HashMap, HasWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  hm.Insert("hair", "brown");

  EXPECT_EQ(hm.Has("color"), true);
  EXPECT_EQ(hm.Has("size"), false);
}

TEST(HashMap, InsertGetWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  hm.Insert("hair", "brown");
  hm.Insert("size", "small");

  EXPECT_EQ(hm["color"], "blue");
  EXPECT_EQ(hm["hair"], "brown");
  EXPECT_EQ(hm["size"], "small");
  EXPECT_EQ(3u, hm.Size());

  std::string s = hm["nothere"];
  EXPECT_EQ(hm["nothere"], s);
  EXPECT_EQ(4u, hm.Size());
}

TEST(HashMap, GetsNonExistingWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  EXPECT_EQ(1u, hm.Size());

  EXPECT_EQ(hm["size"], "");
  EXPECT_EQ(2u, hm.Size());
}

TEST(HashMap, WriteWithSubscriptWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  hm["hair"] = "brown";
  hm["color"] = "yellow";

  EXPECT_EQ(hm["hair"], "brown");
  EXPECT_EQ(hm["color"], "yellow");
  EXPECT_EQ(2u, hm.Size());
}

TEST(HashMap, RemoveWorksCorrectly) {
  DlListHashMap<std::string, std::string> hm;

  hm.Insert("color", "blue");
  hm.Insert("hair", "brown");
  EXPECT_EQ(hm.Has("color"), true);
  EXPECT_EQ(2u, hm.Size());

  hm.Remove("color");
  EXPECT_EQ(hm.Has("color"), false);
  EXPECT_EQ(1u, hm.Size());

  hm.Remove("nothere");
  EXPECT_EQ(1u, hm.Size());
}

TEST(HashMap, HashMapStringInt) {
  DlListHashMap<std::string, int> hm;

  hm.Insert("height", 15);
  hm.Insert("diameter", 4);
  hm.Insert("area", 50);
  EXPECT_EQ(hm.Has("height"), true);
  EXPECT_EQ(3u, hm.Size());

  hm.Remove("height");
  EXPECT_EQ(hm.Has("height"), false);
  EXPECT_EQ(2u, hm.Size());

  EXPECT_EQ(hm.Has("area"), true);
  EXPECT_EQ(hm["area"], 50);
}
} // namespace
