#include "models/Admin.hpp"
#include "models/Client.hpp"
#include <gtest/gtest.h>

TEST(UserTest, PolymorphicRole) {
  std::unique_ptr<User> u1 = std::make_unique<Client>("alice", "hash1");
  std::unique_ptr<User> u2 = std::make_unique<Admin>("bob", "hash2");

  EXPECT_EQ(u1->getRoleName(), "Client");
  EXPECT_EQ(u2->getRoleName(), "Admin");
}


TEST(UserTest, UsernameAccess) {
  Client c("testuser", "123");
  EXPECT_EQ(c.getUsername(), "testuser");
  EXPECT_EQ(c.getPasswordHash(), "123");
}
