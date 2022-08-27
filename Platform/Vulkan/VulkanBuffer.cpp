#include "VulkanBuffer.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "VulkanContext.hpp"

namespace Piksela
{

VulkanVertexBuffer::VulkanVertexBuffer(size_t size)
{
    // Get Vulkan Context from Renderer
    mContext = std::static_pointer_cast<VulkanContext>(Renderer::GetContext());

    // Create the staging buffer
    VkMemoryPropertyFlags stagingBufferProps = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size = size;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(mContext->GetLogicalDevice(), &stagingBufferInfo, nullptr, &mStagingBuffer);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateBuffer failed to create staging buffer.");

    // Allocate memory
    VkMemoryRequirements stagingMemoryReqs;
    vkGetBufferMemoryRequirements(mContext->GetLogicalDevice(), mStagingBuffer, &stagingMemoryReqs);
    uint32_t stagingIndex;
    VkPhysicalDeviceMemoryProperties stagingProps;
    vkGetPhysicalDeviceMemoryProperties(mContext->GetPhysicalDevice(), &stagingProps);
    for (uint32_t i = 0; i < stagingProps.memoryTypeCount; i++)
    {
        bool matchesFilter = stagingMemoryReqs.memoryTypeBits & (1 << i);
        bool matchesProps = (stagingProps.memoryTypes[i].propertyFlags & stagingBufferProps) == stagingBufferProps;
        if (matchesFilter && matchesProps)
        {
            stagingIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo mAllocateInfo = {};
    mAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocateInfo.allocationSize = stagingMemoryReqs.size;
    mAllocateInfo.memoryTypeIndex = stagingIndex;

    result = vkAllocateMemory(mContext->GetLogicalDevice(), &mAllocateInfo, nullptr, &mStagingBufferMemory);
    PK_ASSERT(result == VK_SUCCESS, "vkAllocateMemory failed to allocate memory for the staging buffer.");

    vkBindBufferMemory(mContext->GetLogicalDevice(), mStagingBuffer, mStagingBufferMemory, 0);

    // Create the vertex buffer
    VkMemoryPropertyFlags vertexBufferProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkBufferCreateInfo vertexBufferInfo = {};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.size = size;
    vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    result = vkCreateBuffer(mContext->GetLogicalDevice(), &vertexBufferInfo, nullptr, &mVertexBuffer);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateBuffer failed to create vertex buffer.");

    // Allocate memory
    VkMemoryRequirements vertexMemoryReqs;
    vkGetBufferMemoryRequirements(mContext->GetLogicalDevice(), mVertexBuffer, &vertexMemoryReqs);
    uint32_t vertexIndex;
    VkPhysicalDeviceMemoryProperties vertexProps;
    vkGetPhysicalDeviceMemoryProperties(mContext->GetPhysicalDevice(), &vertexProps);
    for (uint32_t i = 0; i < vertexProps.memoryTypeCount; i++)
    {
        bool matchesFilter = vertexMemoryReqs.memoryTypeBits & (1 << i);
        bool matchesProps = (vertexProps.memoryTypes[i].propertyFlags & vertexBufferProps) == vertexBufferProps;
        if (matchesFilter && matchesProps)
        {
            vertexIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo vertexMemoryAllocateInfo = {};
    vertexMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vertexMemoryAllocateInfo.allocationSize = vertexMemoryReqs.size;
    vertexMemoryAllocateInfo.memoryTypeIndex = vertexIndex;

    result = vkAllocateMemory(mContext->GetLogicalDevice(), &vertexMemoryAllocateInfo, nullptr, &mVertexBufferMemory);
    PK_ASSERT(result == VK_SUCCESS, "vkAllocateMemory failed to allocate memory for the vertex buffer.");

    vkBindBufferMemory(mContext->GetLogicalDevice(), mVertexBuffer, mVertexBufferMemory, 0);

    const std::vector<Vertex> mVertices = {
        { { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { 0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
        { { -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
    };
    void *data;
    vkMapMemory(mContext->GetLogicalDevice(), mStagingBufferMemory, 0, size, 0, &data);
    memcpy(data, mVertices.data(), size);
    vkUnmapMemory(mContext->GetLogicalDevice(), mStagingBufferMemory);
    CopyBuffer(mStagingBuffer, mVertexBuffer, size);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    vkDestroyBuffer(mContext->GetLogicalDevice(), mStagingBuffer, nullptr);
    vkFreeMemory(mContext->GetLogicalDevice(), mStagingBufferMemory, nullptr);
    vkDestroyBuffer(mContext->GetLogicalDevice(), mVertexBuffer, nullptr);
    vkFreeMemory(mContext->GetLogicalDevice(), mVertexBufferMemory, nullptr);
}

void VulkanVertexBuffer::Bind()
{
    mContext->SetVertexBuffer(mVertexBuffer, mVertexBufferMemory);
}

void VulkanVertexBuffer::CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize bufferSize)
{
    VkCommandBufferAllocateInfo cbAllocateInfo = {};
    cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocateInfo.commandPool = mContext->GetCommandPool();
    cbAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mContext->GetLogicalDevice(), &cbAllocateInfo, &commandBuffer);
    VkCommandBufferBeginInfo cbBeginInfo{};
    cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cbBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &cbBeginInfo);

    VkBufferCopy copyRegion = {};
    copyRegion.size = bufferSize;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(mContext->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mContext->GetGraphicsQueue());

    vkFreeCommandBuffers(mContext->GetLogicalDevice(), mContext->GetCommandPool(), 1, &commandBuffer);
}

} // namespace Piksela