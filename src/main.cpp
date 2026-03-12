#include "controllers/WebController.hpp"
#include <drogon/drogon.h>
#include <iostream>

int main() {
  try {
    std::cout << "Consulting Server starting on http://127.0.0.1:8080\n";

    
    
    drogon::app()
        .setDocumentRoot("./public")
        .addListener("127.0.0.1", 8080)
        .run();

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "Unknown Fatal Error\n";
    return 1;
  }
  return 0;
}
