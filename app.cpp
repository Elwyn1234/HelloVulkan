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
    glm::mat2 transform;
    glm::vec2 offset;
    alignas(16) glm::vec3 colour;
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

  glm::vec3 colourBlack { 0.0f, 0.0f, 0.0f };

  void generateSierpinskiTriangle(std::vector<Model::Vertex> &vertices, std::vector<Model::Vertex> containingTriangle, int depth) {
    // Calculate vertices for the triangle (the contained triangle)
    // Top left point
    float ax = (containingTriangle.at(0).position.x + containingTriangle.at(2).position.x) / 2;
    float ay = (containingTriangle.at(0).position.y + containingTriangle.at(2).position.y) / 2;
    
    // Top right point
    float bx = (containingTriangle.at(0).position.x + containingTriangle.at(1).position.x) / 2;
    float by = (containingTriangle.at(0).position.y + containingTriangle.at(1).position.y) / 2;

    // bottom point
    float cx = (containingTriangle.at(1).position.x + containingTriangle.at(2).position.x) / 2;
    float cy = (containingTriangle.at(1).position.y + containingTriangle.at(2).position.y) / 2;
    std::vector<Model::Vertex> triangle = {
      {{ax, ay}, colourBlack },
      {{bx, by}, colourBlack },
      {{cx, cy}, colourBlack },
    };
    vertices.push_back(triangle.at(0));
    vertices.push_back(triangle.at(1));
    vertices.push_back(triangle.at(2));

    if (depth == 7) {
      return;
    }
    depth++;



    // Calculate vertices for the top containing triangle
    // Top point
    ax = containingTriangle.at(0).position.x;
    ay = containingTriangle.at(0).position.y;
    
    // Bottom right point
    bx = triangle.at(1).position.x;
    by = triangle.at(1).position.y;

    // Bottom left point
    cx = triangle.at(0).position.x;
    cy = triangle.at(0).position.y;

    std::vector<Model::Vertex> topContainingTriangle {
      {{ax, ay}, colourBlack },
      {{bx, by}, colourBlack },
      {{cx, cy}, colourBlack },
    };
    generateSierpinskiTriangle(vertices, topContainingTriangle, depth);



    // Calculate vertices for the bottom right containing triangle
    // Top point
    ax = triangle.at(1).position.x;
    ay = triangle.at(1).position.y;
    
    // Bottom right point
    bx = containingTriangle.at(1).position.x;
    by = containingTriangle.at(1).position.y;

    // Bottom left point
    cx = triangle.at(2).position.x;
    cy = triangle.at(2).position.y;

    std::vector<Model::Vertex> bottomRightContainingTriangle {
      {{ax, ay}, colourBlack },
      {{bx, by}, colourBlack },
      {{cx, cy}, colourBlack },
    };
    generateSierpinskiTriangle(vertices, bottomRightContainingTriangle, depth);



    // Calculate vertices for the bottom left containing triangle
    // Top point
    ax = triangle.at(0).position.x;
    ay = triangle.at(0).position.y;
    
    // Bottom right point
    bx = triangle.at(2).position.x;
    by = triangle.at(2).position.y;

    // Bottom left point
    cx = containingTriangle.at(2).position.x;
    cy = containingTriangle.at(2).position.y;

    std::vector<Model::Vertex> bottomLeftContainingTriangle {
      {{ax, ay}, colourBlack },
      {{bx, by}, colourBlack },
      {{cx, cy}, colourBlack },
    };
    generateSierpinskiTriangle(vertices, bottomLeftContainingTriangle, depth);
  }

  void scaleVertices(std::vector<Model::Vertex> &vertices) {
    float xScale = 2;
    float yScale = 1;
    glm::mat2 scaleMat{{xScale, 0}, {0, yScale}};
    for (int i = 0; i < vertices.size(); i++) {
      vertices[i].position = vertices[i].position * scaleMat;
    }
  }

  glm::mat2 calculateRotationMatrix() {
    static float rotation = -.25f * glm::two_pi<float>();
    rotation += 0.01;

    float ix = glm::cos(rotation);
    float iy = glm::sin(rotation);
    float jx = -glm::sin(rotation);
    float jy = glm::cos(rotation);

    glm::vec2 iRotated{ix, iy};
    glm::vec2 jRotated{jx, jy};
    glm::mat2 rotationMat{iRotated, jRotated};
    return rotationMat;
  }

  // TODO:
  // 1) Why does my triangle rotate left instead of right and is that ok?
  // 2) Is the rotation perfect or slightly odd? maybe it needs to be modded (modulus (%)) or perhaps the triangles are being rotated around the origin and that could be making things look skewed?!
  void rotateVertices(std::vector<Model::Vertex> &vertices) {
    float rotation = -.25f * glm::two_pi<float>();
    float ix = glm::cos(rotation);
    float iy = glm::sin(rotation);
    float jx = -glm::sin(rotation);
    float jy = glm::cos(rotation);

    glm::vec2 iRotated{ix, iy};
    glm::vec2 jRotated{jx, jy};
    glm::mat2 rotationMat{iRotated, jRotated};
    for (int i = 0; i < vertices.size(); i++) {
      vertices[i].position = vertices[i].position * rotationMat;
    }
  }
  
  void App::loadModels() {
    glm::vec3 colourPurple { 0.3f, 0.0f, 0.5f };
    std::vector<Model::Vertex> containingTriangle {
      {{0.0f, -0.98f}, colourPurple },
      {{0.98f, 0.98f}, colourPurple },
      {{-0.98f, 0.98f}, colourBlack },
    };
    std::vector<Model::Vertex> vertices {};

    generateSierpinskiTriangle(vertices, containingTriangle, 0);

    vertices.push_back(containingTriangle.at(0));
    vertices.push_back(containingTriangle.at(1));
    vertices.push_back(containingTriangle.at(2));

//    scaleVertices(vertices);
//    rotateVertices(vertices);

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
      clearValues[0].color = {0.0f, 0.0f, 0.0f};
      clearValues[1].depthStencil = {1.0f, 0};
      renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
      renderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
      
      pipeline->bind(commandBuffers[imageIndex]);
      model->bind(commandBuffers[imageIndex]);
      
      for (int j = 0; j < 4; j++) {
        float scalar = frame * 0.01f;
        float xScale = 0.5;
        float yScale = 0.5;
        glm::mat2 scalarMatrix{{xScale, 0}, {0, yScale}};
        SimplePushConstantData pushConstant = {
          {scalarMatrix * calculateRotationMatrix()},
          {-0.5f + scalar, 0.3 + -0.2f * j},
          {0,0,0 /*0.2f * j - scalar * 0.2, 0.0f, 0.0f + scalar * 0.07*/},
        };

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
      if (vkEndCommandBuffer(commandBuffers[imageIndex]) !=VK_SUCCESS) {
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
