#include "model.hpp"
#include "helloVulkanDevice.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace helloVulkan {
  Model::Model(HelloVulkanDevice &device, std::vector<Vertex> &vertices) : helloVulkanDevice{device} {
    createVertexBuffers(vertices);
  }

  Model::~Model() {
    vkDestroyBuffer(helloVulkanDevice.device(), vertexBuffer, NULL);
    vkFreeMemory(helloVulkanDevice.device(), vertexBufferMemory, NULL);
  }

  void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
    helloVulkanDevice.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vertexBuffer,
        vertexBufferMemory);

    void *data;
    vkMapMemory(helloVulkanDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<uint32_t>(bufferSize));
    vkUnmapMemory(helloVulkanDevice.device(), vertexBufferMemory);
  }

  void Model::bind(VkCommandBuffer buffer) {
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, buffers, offsets);
  }

  void Model::draw(VkCommandBuffer buffer) {
    vkCmdDraw(buffer, vertexCount, 1, 0, 0);
  }

  std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> descriptions(1);
    descriptions[0].binding = 0;
    descriptions[0].stride = sizeof(Vertex);
    descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return descriptions;
  }

  std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> descriptions(2);
    descriptions[0].binding = 0;
    descriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    descriptions[0].location = 0;
    descriptions[0].offset = offsetof(Vertex, position);

    descriptions[1].binding = 0;
    descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    descriptions[1].location = 1;
    descriptions[1].offset = offsetof(Vertex, colour);
    return descriptions;
  }
}
