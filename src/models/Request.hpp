#pragma once

#include <chrono>
#include <string>

class Request {
protected:
  int id;
  std::string authorUsername;
  std::string description;
  std::string status;
  std::string type;
  std::string createdAt;

public:
  Request(int id, std::string author, std::string desc, std::string status,
          std::string type, std::string time = "")
      : id(id), authorUsername(std::move(author)), description(std::move(desc)),
        status(std::move(status)), type(std::move(type)),
        createdAt(std::move(time)) {}

  virtual ~Request() = default;

  int getId() const { return id; }
  const std::string &getAuthor() const { return authorUsername; }
  const std::string &getDescription() const { return description; }
  const std::string &getStatus() const { return status; }
  const std::string &getType() const { return type; }
  const std::string &getCreatedAt() const { return createdAt; }

  void setStatus(std::string newStatus) { status = std::move(newStatus); }
  void setCreatedAt(std::string time) { createdAt = std::move(time); }
};
