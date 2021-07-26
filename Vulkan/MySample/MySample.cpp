#include <VulkanApp.h>

#include <iostream>

int main() {
  VulkanApp app;

  try {
    app.run();
  } catch (const std::exception& e) {
    std::cerr << "Exception occurred: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
