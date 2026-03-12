#pragma once

#include "Request.hpp"

class SupportRequest : public Request {
private:
  std::string issueCategory;
  std::string response;

public:
  SupportRequest(int id, std::string author, std::string desc,
                 std::string status, std::string category,
                 std::string resp = "", std::string time = "")
      : Request(id, std::move(author), std::move(desc), std::move(status),
                "Support", std::move(time)),
        issueCategory(std::move(category)), response(std::move(resp)) {}

  const std::string &getCategory() const { return issueCategory; }
  const std::string &getResponse() const { return response; }
  void setResponse(std::string resp) { response = std::move(resp); }
};
