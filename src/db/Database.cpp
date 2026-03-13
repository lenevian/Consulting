#include "Database.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

Database::Database(std::string userPath, std::string reqPath)
    : userDbPath(std::move(userPath)), requestDbPath(std::move(reqPath)) {
  loadUsers();
  loadRequests();
}

Database::~Database() { saveAll(); }

void Database::loadUsers() {
  std::ifstream file(userDbPath);
  if (!file.is_open())
    return;

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::istringstream iss(line);
    std::vector<std::string> fields;
    std::string f;
    while (std::getline(iss, f, ',')) {
      fields.push_back(f);
    }

    if (fields.size() >= 5) {
      std::string role = fields[0];
      std::string name = fields[1];
      std::string hash = fields[2];
      std::string mail = fields[3];
      std::string first = "";
      std::string last = "";

      if (fields.size() == 5) {
        first = fields[4]; 
      } else {
        first = fields[4];
        last = fields[5];
      }

      if (role == "Admin") {
        users.push_back(std::make_shared<Admin>(name, hash, mail, first, last));
      } else {
        users.push_back(
            std::make_shared<Client>(name, hash, mail, first, last));
      }
    }
  }
}

void Database::loadRequests() {
  std::ifstream file(requestDbPath);
  if (!file.is_open())
    return;

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;
    std::istringstream iss(line);
    std::string type, idStr, author, desc, status, extra;

    if (std::getline(iss, type, ',') && std::getline(iss, idStr, ',') &&
        std::getline(iss, author, ',') && std::getline(iss, desc, ',') &&
        std::getline(iss, status, ',') && std::getline(iss, extra)) {

      
      std::string time = "";
      size_t firstPipe = extra.find('|');
      size_t firstComma = extra.find(',');

      
      
      

      
      
      

      int id = std::stoi(idStr);
      if (id >= nextRequestId)
        nextRequestId = id + 1;

      if (type == "Support") {
        std::string category = extra;
        std::string response = "";
        std::string time = "";

        
        size_t p1 = extra.find('|');
        if (p1 != std::string::npos) {
          category = extra.substr(0, p1);
          std::string rest = extra.substr(p1 + 1);
          size_t p2 = rest.find('|');
          if (p2 != std::string::npos) {
            response = rest.substr(0, p2);
            time = rest.substr(p2 + 1);
          } else {
            response = rest;
          }
        }
        requests.push_back(std::make_unique<SupportRequest>(
            id, author, desc, status, category, response, time));
      } else if (type == "Order") {
        std::string serviceType = extra;
        double budget = 0.0;
        std::string time = "";

        
        std::vector<std::string> parts;
        std::stringstream ss(extra);
        std::string p;
        while (std::getline(ss, p, ','))
          parts.push_back(p);

        if (parts.size() >= 2) {
          serviceType = parts[0];
          try {
            budget = std::stod(parts[1]);
          } catch (...) {
          }
          if (parts.size() >= 3)
            time = parts[2];
        } else if (parts.size() == 1) {
          serviceType = parts[0];
        }

        requests.push_back(std::make_unique<OrderRequest>(
            id, author, desc, status, serviceType, budget, time));
      }
    }
  }
}

std::optional<std::shared_ptr<User>>
Database::findUser(const std::string &username) const {
  auto it = std::find_if(users.begin(), users.end(), [&](const auto &u) {
    return u->getUsername() == username;
  });

  if (it != users.end()) {
    return *it;
  }
  return std::nullopt;
}

void Database::addUser(std::shared_ptr<User> user) {
  if (findUser(user->getUsername()).has_value()) {
    throw DatabaseException("User already exists!");
  }
  users.push_back(std::move(user));
}

std::vector<Request *>
Database::findUserRequests(const std::string &username) const {
  std::vector<Request *> userReqs;
  for (const auto &req : requests) {
    if (req->getAuthor() == username) {
      userReqs.push_back(req.get());
    }
  }
  return userReqs;
}

void Database::addRequest(std::unique_ptr<Request> req) {
  if (req->getId() >= nextRequestId) {
    nextRequestId = req->getId() + 1;
  }
  requests.push_back(std::move(req));
}

Request *Database::getRequestById(int id) const {
  for (const auto &r : requests) {
    if (r->getId() == id) {
      return r.get();
    }
  }
  return nullptr;
}

void Database::saveAll() {
  std::ofstream userFile(userDbPath);
  if (!userFile)
    throw DatabaseException("Could not open user file for writing.");
  for (const auto &u : users) {
    userFile << u->getRoleName() << "," << u->getUsername() << ","
             << u->getPasswordHash() << "," << u->getEmail() << ","
             << u->getFirstName() << "," << u->getLastName() << "\n";
  }

  std::ofstream reqFile(requestDbPath);
  if (!reqFile)
    throw DatabaseException("Could not open requests file for writing.");
  for (const auto &r : requests) {
    reqFile << r->getType() << "," << r->getId() << "," << r->getAuthor() << ","
            << r->getDescription() << "," << r->getStatus() << ",";

    if (auto sr = dynamic_cast<SupportRequest *>(r.get())) {
      reqFile << sr->getCategory() << "|" << sr->getResponse() << "|"
              << sr->getCreatedAt() << "\n";
    } else if (auto or_req = dynamic_cast<OrderRequest *>(r.get())) {
      reqFile << or_req->getServiceType() << "," << or_req->getBudget() << ","
              << or_req->getCreatedAt() << "\n";
    }
  }
}
