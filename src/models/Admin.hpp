#pragma once

#include "User.hpp"

class Admin : public User {
public:
  Admin(std::string uname, std::string pwdHash, std::string mail = "",
        std::string first = "", std::string last = "")
      : User(std::move(uname), std::move(pwdHash), std::move(mail),
             std::move(first), std::move(last)) {}

  std::string_view getRoleName() const override { return "Admin"; }
};
