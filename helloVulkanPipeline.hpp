#pragma once

#include "helloVulkanDevice.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace helloVulkan {
  
    struct PipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };
  
  class Pipeline {
    public:
      Pipeline(
          HelloVulkanDevice& device,
          const std::string& vertFilePath,
          const std::string& fragFilePath,
          const PipelineConfigInfo& config);
      ~Pipeline();
      Pipeline(const Pipeline&) = delete;
      void operator=(const Pipeline&) = delete;
      void bind(VkCommandBuffer commandBuffer);
      static PipelineConfigInfo defaultPipelineConfig(
          uint32_t width,
          uint32_t height);
    private:
      static std::vector<char> readFile(
          const std::string& path);
      void createGraphicsPipeline(
          const std::string& vertFilePath, 
          const std::string& fragFilePath,
          const PipelineConfigInfo& config);
      void createShaderModule(
          const std::vector<char>& code,
          VkShaderModule* shaderModule);

      HelloVulkanDevice& helloVulkanDevice;
      VkPipeline graphicsPipeline;
      VkShaderModule vertShaderModule;
      VkShaderModule fragShaderModule;
  };
}
