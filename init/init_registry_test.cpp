#include "init_registry.hpp"

#include <iostream>

#include "base/types/error.hpp"
#include "gtest/gtest.h"

using base::InitArgs;
using base::InitRegistry;

static InitRegistry s_init;
static int __s_reg1__ = s_init.add(
    [](const InitArgs& args) {
      auto arg = absl::any_cast<const char*>(args.at("1"));
      std::cout << "init 1 with str " << arg << std::endl;
      return absl::nullopt;
    },
    [] { std::cout << "destroy 1" << std::endl; }, 0, "first service");

static int __s_reg2__ = s_init.add(
    [](const InitArgs& args) {
      auto arg = absl::any_cast<int>(args.at("2"));
      std::cout << "init 2 with number " << arg << std::endl;
      return absl::nullopt;
    },
    [] { std::cout << "destory 2" << std::endl; }, -1, "second service");

TEST(init_registry, simple) {
  InitArgs args = {{"1", "1"}, {"2", 2}};
  s_init.init(args);
  s_init.destory();
}

static InitRegistry s_init_error("error");
static int __s_reg_e1__ = s_init_error.add(
    [](const InitArgs& args) {
      auto arg = absl::any_cast<const char*>(args.at("1"));
      std::cout << "init 1 with str " << arg << std::endl;
      return absl::nullopt;
    },
    [] { std::cout << "destroy 1" << std::endl; }, 0, "first service");

static int __s_reg_e2__ = s_init_error.add(
    [](const InitArgs& args) {
      auto arg = absl::any_cast<int>(args.at("2"));
      std::cout << "init 2 with number " << arg << std::endl;
      return absl::nullopt;
    },
    [] { std::cout << "destory 2" << std::endl; }, -1, "second service");

static int __s_reg_error__ = s_init_error.add(
    [](const InitArgs& args) {
      return base::ErrorCode({-1, "test error"});
    },
    [] { std::cout << "destory 2" << std::endl; }, 2, "error service");

TEST(init_registry, error) {
  InitArgs args = {{"1", "1"}, {"2", 2}};
  auto error = s_init_error.init(args);
  ASSERT_TRUE(error.has_value());
}