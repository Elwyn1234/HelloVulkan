#pragma once

#include "helloVulkanWindow.hpp"
#include "helloVulkanPipeline.hpp"
#include "helloVulkanDevice.hpp"
#include "helloVulkanSwapChain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace helloVulkan {

  class App {
    public:
      static const int WIDTH = 1000;
      static const int HEIGHT = 750;

      App();
      ~App();

      App(const App &) = delete;
      App &operator=(const App &) = delete;

      void run();

    private:
      void sierpinskiTriangle();
      void loadModels();
      void createPipeline();
      void createPipelineLayout();
      void createCommandBuffers();
      void drawFrame();
      void recordCommandBuffer(int imageIndex);

      HelloVulkanWindow helloVulkanWindow{
            WIDTH,
            HEIGHT,
            "elwynn" };
      HelloVulkanDevice helloVulkanDevice{helloVulkanWindow};
      HelloVulkanSwapChain helloVulkanSwapChain{ helloVulkanDevice, helloVulkanWindow.getExtent() };
      std::unique_ptr<Pipeline> pipeline;
      VkPipelineLayout pipelineLayout;
      std::vector<VkCommandBuffer> commandBuffers;
      std::unique_ptr<Model> model;
  };
}
