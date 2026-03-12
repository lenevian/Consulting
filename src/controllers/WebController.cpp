#include "WebController.hpp"
#include "utils/ConstexprUtils.hpp"
#include <iostream>

using namespace drogon;

WebController::WebController() {
  db = std::make_unique<Database>("users.txt", "requests.txt");
}

std::string WebController::resolvePath(const std::string &path) {
  if (path == "/" || path == "")
    return "index.html";
  if (path == "/login")
    return "login.html";
  if (path == "/register")
    return "register.html";
  if (path == "/profile")
    return "profile.html";
  if (path == "/orders")
    return "orders.html";
  if (path == "/admin")
    return "admin.html";
  if (path == "/support")
    return "support.html";
  return path;
}

void WebController::serveFile(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  std::string filePath = "./public/" + resolvePath(req->path());
  auto resp = HttpResponse::newFileResponse(filePath);
  if (!resp) {
    resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k404NotFound);
    resp->setBody("404 Not Found");
  }
  callback(resp);
}

void WebController::login(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  auto resp = HttpResponse::newHttpResponse();

  if (!json || !json->isMember("username") || !json->isMember("password")) {
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Invalid request\"}");
    callback(resp);
    return;
  }

  std::string username = (*json)["username"].asString();
  std::string password = (*json)["password"].asString();

  auto userOpt = db->findUser(username);
  if (!userOpt) {
    resp->setStatusCode(k401Unauthorized);
    resp->setBody("{\"error\": \"User not found\"}");
    callback(resp);
    return;
  }

  uint32_t passHash = consulting::hash_string(password);
  if (std::to_string(passHash) != userOpt.value()->getPasswordHash()) {
    resp->setStatusCode(k401Unauthorized);
    resp->setBody("{\"error\": \"Incorrect password\"}");
    callback(resp);
    return;
  }

  Json::Value ret;
  ret["username"] = userOpt.value()->getUsername();
  ret["firstName"] = userOpt.value()->getFirstName();
  ret["lastName"] = userOpt.value()->getLastName();
  ret["email"] = userOpt.value()->getEmail();
  ret["role"] = std::string(userOpt.value()->getRoleName());

  auto jsonResp = HttpResponse::newHttpJsonResponse(ret);
  callback(jsonResp);
}

void WebController::registerUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  auto resp = HttpResponse::newHttpResponse();

  if (!json || !json->isMember("username") || !json->isMember("password")) {
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Invalid request\"}");
    callback(resp);
    return;
  }

  std::string username = (*json)["username"].asString();
  std::string password = (*json)["password"].asString();
  std::string email = (*json)["email"].asString();
  std::string firstName = (*json)["firstName"].asString();
  std::string lastName = (*json)["lastName"].asString();
  uint32_t passHash = consulting::hash_string(password);

  try {
    db->addUser(std::make_shared<Client>(username, std::to_string(passHash),
                                         email, firstName, lastName));
    db->saveAll(); 
    auto jsonResp = HttpResponse::newHttpJsonResponse(Json::Value("success"));
    callback(jsonResp);
  } catch (const DatabaseException &e) {
    resp->setStatusCode(k409Conflict);
    resp->setBody("{\"error\": \"" + std::string(e.what()) + "\"}");
    callback(resp);
  }
}

void WebController::getOrders(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  std::string username = req->getHeader("X-Username");
  if (username.empty()) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    callback(resp);
    return;
  }

  auto reqs = db->findUserRequests(username);
  Json::Value ret(Json::arrayValue);

  for (const auto *r : reqs) {
    Json::Value item;
    item["id"] = r->getId();
    item["type"] = r->getType();
    item["description"] = r->getDescription();
    item["status"] = r->getStatus();
    item["time"] = r->getCreatedAt();

    if (auto sr = dynamic_cast<const SupportRequest *>(r)) {
      item["category"] = sr->getCategory();
      item["response"] = sr->getResponse();
    } else if (auto or_req = dynamic_cast<const OrderRequest *>(r)) {
      item["service"] = or_req->getServiceType();
      item["budget"] = or_req->getBudget();
      item["extra"] = std::to_string(or_req->getBudget());
    }

    ret.append(item);
  }

  auto jsonResp = HttpResponse::newHttpJsonResponse(ret);
  callback(jsonResp);
}

void WebController::createOrder(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  auto resp = HttpResponse::newHttpResponse();

  if (!json || !json->isMember("username") || !json->isMember("description") ||
      !json->isMember("service")) {
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Missing fields\"}");
    callback(resp);
    return;
  }

  std::string author = (*json)["username"].asString();
  std::string desc = (*json)["description"].asString();
  std::string svc = (*json)["service"].asString();
  double budget = 0.0;
  try {
    std::string bStr = (*json)["budget"].asString();
    if (!bStr.empty())
      budget = std::stod(bStr);
  } catch (...) {
    budget = 0.0;
  }

  try {
    db->addRequest(std::make_unique<OrderRequest>(db->getNextRequestId(),
                                                  author, desc, "Pending", svc,
                                                  budget, getTimeStr()));
    db->saveAll();
    auto jsonResp = HttpResponse::newHttpJsonResponse(Json::Value("success"));
    callback(jsonResp);
  } catch (const std::exception &e) {
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("{\"error\": \"Server error\"}");
    callback(resp);
  }
}

void WebController::getAllOrders(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  std::string adminUser = req->getHeader("X-Username");
  auto userOpt = db->findUser(adminUser);

  if (!userOpt || userOpt.value()->getRoleName() != "Admin") {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k403Forbidden);
    resp->setBody("{\"error\": \"Access denied\"}");
    callback(resp);
    return;
  }

  auto &reqs = db->getAllRequests();
  Json::Value ret(Json::arrayValue);

  for (const auto &r : reqs) {
    Json::Value item;
    item["id"] = r->getId();
    item["author"] = r->getAuthor();

    auto authorObj = db->findUser(r->getAuthor());
    if (authorObj) {
      item["firstName"] = authorObj.value()->getFirstName();
      item["lastName"] = authorObj.value()->getLastName();
      item["email"] = authorObj.value()->getEmail();
    }

    item["type"] = r->getType();
    item["description"] = r->getDescription();
    item["status"] = r->getStatus();
    item["time"] = r->getCreatedAt();

    if (auto sr = dynamic_cast<const SupportRequest *>(r.get())) {
      item["service"] = "Support: " + sr->getCategory();
      item["response"] = sr->getResponse();
    } else if (auto or_req = dynamic_cast<const OrderRequest *>(r.get())) {
      item["service"] = or_req->getServiceType();
      item["budget"] = or_req->getBudget();
      item["extra"] = std::to_string(or_req->getBudget());
    }

    ret.append(item);
  }

  auto jsonResp = HttpResponse::newHttpJsonResponse(ret);
  callback(jsonResp);
}

void WebController::updateOrderStatus(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  std::string adminUser = req->getHeader("X-Username");
  auto userOpt = db->findUser(adminUser);

  if (!userOpt || userOpt.value()->getRoleName() != "Admin") {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k403Forbidden);
    resp->setBody("{\"error\": \"Access denied\"}");
    callback(resp);
    return;
  }

  if (!json || !json->isMember("id") || !json->isMember("status")) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Invalid parameters\"}");
    callback(resp);
    return;
  }

  int id = (*json)["id"].asInt();
  std::string newStatus = (*json)["status"].asString();

  auto *r = db->getRequestById(id);
  if (!r) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k404NotFound);
    callback(resp);
    return;
  }

  r->setStatus(newStatus);
  db->saveAll();

  auto jsonResp = HttpResponse::newHttpJsonResponse(Json::Value("success"));
  callback(jsonResp);
}
void WebController::createSupport(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  auto resp = HttpResponse::newHttpResponse();

  if (!json || !json->isMember("username") || !json->isMember("description") ||
      !json->isMember("category")) {
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Missing fields\"}");
    callback(resp);
    return;
  }

  std::string author = (*json)["username"].asString();
  std::string desc = (*json)["description"].asString();
  std::string category = (*json)["category"].asString();

  try {
    db->addRequest(
        std::make_unique<SupportRequest>(db->getNextRequestId(), author, desc,
                                         "Open", category, "", getTimeStr()));
    db->saveAll();
    auto jsonResp = HttpResponse::newHttpJsonResponse(Json::Value("success"));
    callback(jsonResp);
  } catch (const std::exception &e) {
    resp->setStatusCode(k500InternalServerError);
    resp->setBody("{\"error\": \"Server error\"}");
    callback(resp);
  }
}

void WebController::respondToSupport(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  std::string adminUser = req->getHeader("X-Username");
  auto userOpt = db->findUser(adminUser);

  if (!userOpt || userOpt.value()->getRoleName() != "Admin") {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k403Forbidden);
    callback(resp);
    return;
  }

  if (!json || !json->isMember("id") || !json->isMember("response")) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  int id = (*json)["id"].asInt();
  std::string supportResp = (*json)["response"].asString();

  auto *r = db->getRequestById(id);
  if (!r) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k404NotFound);
    callback(resp);
    return;
  }

  if (auto *sr = dynamic_cast<SupportRequest *>(r)) {
    sr->setResponse(supportResp);
    sr->setStatus("Resolved");
    db->saveAll();
    auto jsonResp = HttpResponse::newHttpJsonResponse(Json::Value("success"));
    callback(jsonResp);
  } else {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k400BadRequest);
    resp->setBody("{\"error\": \"Not a support request\"}");
    callback(resp);
  }
}

std::string WebController::getTimeStr() const {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  struct tm gmt;
#ifdef _WIN32
  gmtime_s(&gmt, &in_time_t);
#else
  gmtime_r(&in_time_t, &gmt);
#endif
  char buf[80];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", &gmt);
  return std::string(buf);
}
