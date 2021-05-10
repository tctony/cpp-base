#include "base/error/error.hpp"

#include <gtest/gtest.h>

using namespace base::error;

TEST(Error, no_error) {
  auto ec1 = std::error_code();
  auto ec2 = Error(noError);
  EXPECT_FALSE(ec1);
  EXPECT_FALSE(ec2);
}

TEST(Error, equal) {
  auto ec1 = std::error_code(999, std::system_category());
  auto ec2 = std::error_code(999, std::generic_category());
  auto ec3 = Error(static_cast<ErrorCode>(999));
  EXPECT_NE(ec1, ec2);
  EXPECT_EQ(ec1, ec3);
  EXPECT_EQ(ec2, ec3);
  std::cout << "ec1.category().name(): " << ec1.category().name() << std::endl;
  std::cout << "ec2.category().name(): " << ec2.category().name() << std::endl;
  std::cout << "ec3.category().name(): " << ec3.category().name() << std::endl;
  std::cout << "ec1.message(): " << ec1.message() << std::endl;
  std::cout << "ec2.message(): " << ec2.message() << std::endl;
  std::cout << "ec3.message(): " << ec3.message() << std::endl;
}
