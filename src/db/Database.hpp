#pragma once

#include "../exceptions/ConsultingException.hpp"
#include "../models/Admin.hpp"
#include "../models/Client.hpp"
#include "../models/OrderRequest.hpp"
#include "../models/Request.hpp"
#include "../models/SupportRequest.hpp"
#include "../models/User.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Database {
private:
  std::string userDbPath;
  std::string requestDbPath;

  
  
  std::vector<std::shared_ptr<User>> users;
  std::vector<std::unique_ptr<Request>> requests;
  int nextRequestId = 100;

  void loadUsers();
  void loadRequests();

public:
  Database(std::string userPath, std::string reqPath);
  ~Database();

  
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  std::optional<std::shared_ptr<User>>
  findUser(const std::string &username) const;
  void addUser(std::shared_ptr<User> user);

  std::vector<Request *> findUserRequests(const std::string &username) const;
  void addRequest(std::unique_ptr<Request> req);
  int getNextRequestId() const { return nextRequestId; }
  const std::vector<std::unique_ptr<Request>> &getAllRequests() const {
    return requests;
  }

  
  Request *getRequestById(int id) const;

  void saveAll();
};
