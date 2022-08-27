#include <volk.h>

#include "Engines/Rendering/Buffer.hpp"
#include "VulkanContext.hpp"

namespace Piksela
{

class VulkanVertexBuffer : public VertexBuffer
{
public:
    VulkanVertexBuffer(size_t size);
    ~VulkanVertexBuffer();

    virtual void Bind() override;

private:
    void CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize bufferSize);

private:
    std::shared_ptr<VulkanContext> mContext;

    VkBuffer mStagingBuffer;
    VkDeviceMemory mStagingBufferMemory;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
};

} // namespace Piksela