#pragma once

#include "helloVulkanDevice.hpp"
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace helloVulkan {
  class Model {
    public:
      struct Vertex {
        glm::vec4 position;
        glm::vec3 colour;
        
        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
      };

      Model(HelloVulkanDevice &device, std::vector<Vertex> &vertices);
      ~Model();

      Model(const Model &) = delete;
      Model &operator=(const Model &) = delete;

      void bind(VkCommandBuffer buffer);
      void draw(VkCommandBuffer buffer);

    private:
      void createVertexBuffers(const std::vector<Vertex> &vertices);
      HelloVulkanDevice& helloVulkanDevice;
      VkBuffer vertexBuffer;
      VkDeviceMemory vertexBufferMemory;
      uint32_t vertexCount;

  };
}
