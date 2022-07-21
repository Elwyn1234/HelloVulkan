#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace helloVulkan {

  class HelloVulkanWindow {
    public:
      HelloVulkanWindow(int h, int w, std::string name);
      ~HelloVulkanWindow();
      HelloVulkanWindow(const HelloVulkanWindow &) = delete;
      HelloVulkanWindow &operator=(const HelloVulkanWindow &) = delete;
      bool shouldClose() { return glfwWindowShouldClose(window); }
      VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(width) }; }
      void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    private:
      const int width;
      const int height;
      const std::string name;
      GLFWwindow *window;
      void init();
  };
}
