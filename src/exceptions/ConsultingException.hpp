#pragma once

#include <stdexcept>
#include <string>

class ConsultingException : public std::runtime_error {
public:
  explicit ConsultingException(const std::string &message)
      : std::runtime_error(message) {}
};

class AuthException : public ConsultingException {
public:
  explicit AuthException(const std::string &message)
      : ConsultingException("Auth Error: " + message) {}
};

class ValidationException : public ConsultingException {
public:
  explicit ValidationException(const std::string &message)
      : ConsultingException("Validation Error: " + message) {}
};

class DatabaseException : public ConsultingException {
public:
  explicit DatabaseException(const std::string &message)
      : ConsultingException("Database Error: " + message) {}
};
