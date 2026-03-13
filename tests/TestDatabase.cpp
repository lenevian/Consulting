#include "db/Database.hpp"
#include <cstdio>
#include <gtest/gtest.h>

class DatabaseTest : public ::testing::Test {
protected:
  std::string uPath = "test_users.txt";
  std::string rPath = "test_reqs.txt";

  void SetUp() override {
    std::remove(uPath.c_str());
    std::remove(rPath.c_str());
  }

  void TearDown() override {
    std::remove(uPath.c_str());
    std::remove(rPath.c_str());
  }
};

TEST_F(DatabaseTest, AddAndFindUser) {
  Database db(uPath, rPath);
  db.addUser(std::make_shared<Client>("testuser", "hash123"));

  auto u = db.findUser("testuser");
  ASSERT_TRUE(u.has_value());
  EXPECT_EQ(u.value()->getUsername(), "testuser");
  EXPECT_EQ(u.value()->getPasswordHash(), "hash123");
}

TEST_F(DatabaseTest, PreventDuplicateUser) {
  Database db(uPath, rPath);
  db.addUser(std::make_shared<Client>("testuser", "hash123"));

  EXPECT_THROW(db.addUser(std::make_shared<Admin>("testuser", "xyz")),
               DatabaseException);
}

TEST_F(DatabaseTest, DatabasePersistence) {
  {
    Database db(uPath, rPath);
    db.addUser(std::make_shared<Admin>("admin1", "adminhash"));
    db.addRequest(
        std::make_unique<SupportRequest>(1, "admin1", "Help", "Open", "Tech"));
    
  }

  {
    Database db2(uPath, rPath);
    auto u = db2.findUser("admin1");
    ASSERT_TRUE(u.has_value());
    EXPECT_EQ(u.value()->getRoleName(), "Admin");

    auto reqs = db2.findUserRequests("admin1");
    ASSERT_EQ(reqs.size(), 1);
    EXPECT_EQ(reqs[0]->getType(), "Support");
  }
}
