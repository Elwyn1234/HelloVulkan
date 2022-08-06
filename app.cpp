#include "app.hpp"
#include "helloVulkanDevice.hpp"
#include "helloVulkanPipeline.hpp"
#include "model.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cstdio>
#include <glm/fwd.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace helloVulkan {

  struct SimplePushConstantData {
    glm::mat4 transform;
  };

  App::App() {
    loadModels();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
  }

  App::~App() {
    vkDestroyPipelineLayout(helloVulkanDevice.device(), pipelineLayout, nullptr);
  }

  void App::run() {
    while(!helloVulkanWindow.shouldClose()) {
      glfwPollEvents();
      drawFrame();
    }
    vkDeviceWaitIdle(helloVulkanDevice.device());
  }

  enum axis {
    axisx,
    axisy,
    axisz
  };

  glm::mat4 calculateRotationMatrix(axis axisOfRotation) {
    static float rotation = -.25f * glm::two_pi<float>();
    rotation += 0.01;

    float ix = glm::cos(rotation);
    float iy = glm::sin(rotation);
    float iz = glm::sin(rotation);
    float jx = -glm::sin(rotation);
    float jy = glm::cos(rotation);
    float jz = glm::cos(rotation);
    float kx = -glm::sin(rotation);
    float ky = glm::cos(rotation);
    float kz = glm::cos(rotation);

    glm::vec4 iRotated;
    glm::vec4 jRotated;
    glm::vec4 kRotated;
    glm::vec4 translationRotated;

    if (axisOfRotation == axisx) {
      iRotated = {1.0f, 0.0f, 0.0f, 0.0f};
      jRotated = {0.0f, jy, jz, 0.0f};
      kRotated = {0.0f, ky, kz, 0.0f};
      translationRotated = {0.0f, 0.0f, 0.0f, 1.0f};
    }

    glm::mat4 rotationMat{iRotated, jRotated, kRotated, translationRotated};
    return rotationMat;
  }

  // TODO:
  // 1) Why does my triangle rotate left instead of right and is that ok?
  // 2) Is the rotation perfect or slightly odd? maybe it needs to be modded (modulus (%)) or perhaps the triangles are being rotated around the origin and that could be making things look skewed?!
  // 3) when I translate before rotate, the visuals are not as I would expect?!
  // 4) Does the attribute description need to match the Vertex.position size?
  
  void App::loadModels() {
    glm::vec3 colourPurple { 0.3f, 0.0f, 0.5f };
    std::vector<Model::Vertex> vertices = {
      {{-0.5f, -0.5f, 0.5f, 1.0f}, colourPurple},
      {{0.5f, -0.5f, 0.5f, 1.0f}, colourPurple},
      {{0.5f, 0.5f, 0.5f, 1.0f}, colourPurple},
      {{-0.5f, -0.5f, 0.5f, 1.0f}, colourPurple},
      {{0.5f, 0.5f, 0.5f, 1.0f}, colourPurple},
      {{-0.5f, 0.5f, 0.5f, 1.0f}, colourPurple},
    };

    model = std::make_unique<Model>(helloVulkanDevice, vertices);
  }

  void App::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = NULL;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(helloVulkanDevice.device(), &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create pipeline layout");
    };
  }

  void App::createPipeline() {
    auto pipelineConfigInfo = Pipeline::defaultPipelineConfig(helloVulkanSwapChain.width(), helloVulkanSwapChain.height());
    pipelineConfigInfo.renderPass = helloVulkanSwapChain.getRenderPass();
    pipelineConfigInfo.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Pipeline>(
        helloVulkanDevice,
        "shaders/simpleShader.vert.spv",
        "shaders/simpleShader.frag.spv",
        pipelineConfigInfo
        );
  }

  void App::createCommandBuffers() {
    commandBuffers.resize(helloVulkanSwapChain.imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = helloVulkanDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    
    if (vkAllocateCommandBuffers(helloVulkanDevice.device(), &allocInfo, commandBuffers.data()) !=VK_SUCCESS) {
      throw std::runtime_error("Failed to create command buffers");
    };
  }

  void App::recordCommandBuffer(int imageIndex) {
      static int frame = 0;
      frame = (frame + 1) % 100;

      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
      };

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = helloVulkanSwapChain.getRenderPass();
      renderPassInfo.framebuffer = helloVulkanSwapChain.getFrameBuffer(imageIndex);

      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = helloVulkanSwapChain.getSwapChainExtent();

      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {0.5f, 0.5f, 0.5f};
      clearValues[1].depthStencil = {1.0f, 0};
      renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
      
      pipeline->bind(commandBuffers[imageIndex]);
      model->bind(commandBuffers[imageIndex]);
      
      for (int j = 0; j < 4; j++) {
        float scalar = frame * 0.01f;
        float xScale = 0.5f;
        float yScale = 0.5f;
        float zScale = 0.5f;

        SimplePushConstantData pushConstant = { {
          xScale, 0.0f, 0.0f, 0.0f,
          0.0f, yScale, 0.0f, 0.0f,
          0.0f, 0.0f, zScale, 0.0f,
          0.0f, 0.0f, 0.5f, 1.0f
        }, };
        pushConstant.transform = pushConstant.transform * calculateRotationMatrix(axisx);

        vkCmdPushConstants(
            commandBuffers[imageIndex],
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &pushConstant);

        model->draw(commandBuffers[imageIndex]);
      }

      vkCmdEndRenderPass(commandBuffers[imageIndex]);
      if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        std::runtime_error("Failed to end command buffer");
      }
  }

  void App::drawFrame() {
    uint32_t imageIndex;
    auto result = helloVulkanSwapChain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("Failed to acquire swap chain image");
    }

    recordCommandBuffer(imageIndex);
    
    result = helloVulkanSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("Failed to present swap chain image");
    }
  }
}
