#include "string_util.hpp"

#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

using std::string;

using base::util::ltrim;
using base::util::rtrim;
using base::util::split;
using base::util::toLowerCase;
using base::util::toUpperCase;
using base::util::trim;

TEST(StringCase, toLower) {
  string str0("TONY");
  string str1("Tony");
  string str2("tony");

  string result("tony");
  EXPECT_EQ(result, toLowerCase(str0));
  EXPECT_EQ(result, toLowerCase(str1));
  EXPECT_EQ(result, toLowerCase(str2));

  string str3("Tony_");
  EXPECT_EQ("tony_", toLowerCase(str3));
}

TEST(StringCase, toUpper) {
  string str0("TONY");
  string str1("Tony");
  string str2("tony");

  string result("TONY");
  EXPECT_EQ(result, toUpperCase(str0));
  EXPECT_EQ(result, toUpperCase(str1));
  EXPECT_EQ(result, toUpperCase(str2));

  string str3("Tony_");
  EXPECT_EQ("TONY_", toUpperCase(str3));
}

TEST(StringSplit, split) {
  const std::string str = "a,b,c,d";
  std::vector<std::string> vr;
  size_t n;

  n = split(vr, str, ",");
  EXPECT_EQ(n, 4);
  EXPECT_EQ(vr[0], "a");
  EXPECT_EQ(vr[1], "b");
  EXPECT_EQ(vr[2], "c");
  EXPECT_EQ(vr[3], "d");

  n = split(vr, str, ",", 1);
  EXPECT_EQ(n, 2);
  EXPECT_EQ(vr[0], "a");
  EXPECT_EQ(vr[1], "b,c,d");
}

TEST(StrTrim, trim) {
  const std::string str = "_-'aaa'-_";

  EXPECT_EQ(trim(str, "_-"), "'aaa'");
  EXPECT_EQ(rtrim(str, "_-"), "_-'aaa'");
  EXPECT_EQ(ltrim(str, "_-"), "'aaa'-_");
}
