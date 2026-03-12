#pragma once

#include "Request.hpp"

class OrderRequest : public Request {
private:
  std::string serviceType;
  double budget;

public:
  OrderRequest(int id, std::string author, std::string desc, std::string status,
               std::string service, double b, std::string time = "")
      : Request(id, std::move(author), std::move(desc), std::move(status),
                "Order", std::move(time)),
        serviceType(std::move(service)), budget(b) {}

  const std::string &getServiceType() const { return serviceType; }
  double getBudget() const { return budget; }
};
