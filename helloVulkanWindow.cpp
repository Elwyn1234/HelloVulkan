#include "helloVulkanWindow.hpp"

namespace helloVulkan {

  HelloVulkanWindow::HelloVulkanWindow(int w, int h, std::string name) : width{w}, height{h}, name{name} {
    init();
  }

  HelloVulkanWindow::~HelloVulkanWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void HelloVulkanWindow::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
  }
  void HelloVulkanWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    glfwCreateWindowSurface(instance, window, nullptr, surface);
  }
}
