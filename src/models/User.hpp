#pragma once

#include <memory>
#include <string>
#include <string_view>

class User {
protected:
  std::string username;
  std::string passwordHash;
  std::string email;
  std::string firstName;
  std::string lastName;

public:
  User(std::string uname, std::string pwdHash, std::string mail = "",
       std::string first = "", std::string last = "")
      : username(std::move(uname)), passwordHash(std::move(pwdHash)),
        email(std::move(mail)), firstName(std::move(first)),
        lastName(std::move(last)) {}

  virtual ~User() = default;

  virtual std::string_view getRoleName() const = 0;

  const std::string &getUsername() const { return username; }
  const std::string &getPasswordHash() const { return passwordHash; }
  const std::string &getEmail() const { return email; }
  const std::string &getFirstName() const { return firstName; }
  const std::string &getLastName() const { return lastName; }
};
