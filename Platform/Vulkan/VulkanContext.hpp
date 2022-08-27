#pragma once

#include <volk.h>
#include <glm/glm.hpp>

#include "Core/Core.hpp"
#include "Engines/Rendering/Context.hpp"

namespace Piksela
{

struct Vertex
{
    glm::vec3 Position;
    glm::vec4 Color;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, Color);

        return attributeDescriptions;
    }
};

struct ShaderMatrices
{
    glm::mat4 View;
    glm::mat4 Model;
    glm::mat4 Projection;
};

class VulkanContext : public Context
{
public:
    VulkanContext(const std::shared_ptr<Window> &window);
    ~VulkanContext();

    virtual void DrawFrame() override;
    virtual void ResizeViewport() override;
    virtual void WaitIdle() override;

    inline VkCommandPool GetCommandPool()
    {
        return mCommandPool;
    }
    inline VkQueue GetGraphicsQueue()
    {
        return mGraphicsQueue;
    }
    inline VkPhysicalDevice GetPhysicalDevice()
    {
        return mPhysicalDevice;
    }
    inline VkDevice GetLogicalDevice()
    {
        return mLogicalDevice;
    }
    inline void SetVertexBuffer(VkBuffer buffer, VkDeviceMemory memory)
    {
        mVertexBuffer = buffer;
        mVertexBufferMemory = memory;
    }

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

    void CreateInstance();
    bool CheckValidationLayerSupport(const std::vector<const char *> &validationLayers);
    std::vector<const char *> GetRequiredExtensions();

    void CreateDebugMessenger();
    void CreateSurface();

    void CreatePhysicalDevice();
    void CreateLogicalDevice();

    void RecreateSwapChain();
    void CreateSwapChain();
    void CleanupSwapChain();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();

    std::string ReadSpirVFile(const std::string &path);
    VkShaderModule CreateShaderModule(const std::string &source);

    void CreateFramebuffers();

    // TODO: Refactor out vertex buffers
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize bufferSize);
    void UpdateUniformBuffer(uint32_t currentFrame);

    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void CreateSyncObjects();

    void CreateDescriptorPool();
    void CreateDescriptorSets();

    uint32_t FindMemoryType(uint32_t filter, VkMemoryPropertyFlags properties);

private:
    std::shared_ptr<Window> mWindow;

    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mLogicalDevice;

    VkQueue mGraphicsQueue;
    uint32_t mGraphicsQueueFamilyIndex;
    VkQueue mPresentQueue;
    uint32_t mPresentQueueFamilyIndex;

    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;
    VkFormat mSwapChainFormat;
    VkExtent2D mSwapChainExtent;

    VkRenderPass mRenderPass;
    VkDescriptorSetLayout mDescriptorSetLayout;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;
    std::vector<VkFramebuffer> mFramebuffers;

    const std::vector<uint16_t> mIndices = {
        0, 1, 2, 2, 3, 0
    };
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VkDeviceMemory> mUniformBufferMemories;

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;

    VkDescriptorPool mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    bool mShouldResize;
};

} // namespace Piksela