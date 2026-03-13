#pragma once
#include "../db/Database.hpp"
#include <drogon/HttpController.h>
#include <memory>

using namespace drogon;

class WebController : public drogon::HttpController<WebController> {
public:
    WebController();

    METHOD_LIST_BEGIN

    ADD_METHOD_TO(WebController::login, "/api/login", Post);
    ADD_METHOD_TO(WebController::registerUser, "/api/register", Post);
    ADD_METHOD_TO(WebController::getOrders, "/api/orders", Get);
    ADD_METHOD_TO(WebController::getAllOrders, "/api/admin/orders", Get);
    ADD_METHOD_TO(WebController::updateOrderStatus, "/api/admin/orders/update",
    Post);
    ADD_METHOD_TO(WebController::respondToSupport, "/api/admin/support/respond",
    Post);
    ADD_METHOD_TO(WebController::createOrder, "/api/orders/create", Post);
    ADD_METHOD_TO(WebController::createSupport, "/api/support/create", Post);




    ADD_METHOD_TO(WebController::serveFile, "/", Get);
    ADD_METHOD_TO(WebController::serveFile, "/login", Get);
    ADD_METHOD_TO(WebController::serveFile, "/register", Get);
    ADD_METHOD_TO(WebController::serveFile, "/profile", Get);
    ADD_METHOD_TO(WebController::serveFile, "/orders", Get);
    ADD_METHOD_TO(WebController::serveFile, "/admin", Get);
    ADD_METHOD_TO(WebController::serveFile, "/support", Get);
    METHOD_LIST_END

    void login(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback);
    void registerUser(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void getOrders(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
    void getAllOrders(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void
    updateOrderStatus(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
    void
    respondToSupport(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
    void createOrder(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
    void createSupport(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);

    void serveFile(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);

private:
    std::unique_ptr<Database> db;
    std::string resolvePath(const std::string &path);
    std::string getTimeStr() const;
};
