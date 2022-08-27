#include <chrono>
#include <fstream>

#ifdef PK_GLFW
#define GLFW_INCLUDE_VULKAN
#define VK_NO_PROTOTYPES
#include <GLFW/glfw3.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <volk.h>

#include "Core/Core.hpp"
#include "Platform/Window.hpp"
#include "VulkanContext.hpp"

namespace Piksela
{

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
{
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        CoreLogger::Error("[Vulkan]: {0}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        CoreLogger::Warn("[Vulkan]: {0}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        CoreLogger::Info("[Vulkan]: {0}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        CoreLogger::Trace("[Vulkan]: {0}", pCallbackData->pMessage);
    }
    return VK_FALSE;
}

VulkanContext::VulkanContext(const std::shared_ptr<Window> &window)
{
    PK_ASSERT(volkInitialize() == VK_SUCCESS, "Failed to initialize volk.");
    mWindow = window;
    mShouldResize = false;

    CreateInstance();
    CreateDebugMessenger();
    CreateSurface();
    CreatePhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();

    CreateIndexBuffer();
    CreateUniformBuffers();

    CreateDescriptorPool();
    CreateDescriptorSets();
}

VulkanContext::~VulkanContext()
{
    WaitIdle();
    vkDestroyBuffer(mLogicalDevice, mIndexBuffer, nullptr);
    vkFreeMemory(mLogicalDevice, mIndexBufferMemory, nullptr);

    CleanupSwapChain();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyBuffer(mLogicalDevice, mUniformBuffers[i], nullptr);
        vkFreeMemory(mLogicalDevice, mUniformBufferMemories[i], nullptr);
    }
    vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mLogicalDevice, mDescriptorSetLayout, nullptr);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(mLogicalDevice, mInFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);
    vkDestroyRenderPass(mLogicalDevice, mRenderPass, nullptr);
    vkDestroyPipelineLayout(mLogicalDevice, mPipelineLayout, nullptr);
    vkDestroyPipeline(mLogicalDevice, mGraphicsPipeline, nullptr);
    vkDestroyDevice(mLogicalDevice, nullptr);

    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

void VulkanContext::DrawFrame()
{
    static uint32_t currentFrame = 0;

    vkWaitForFences(mLogicalDevice, 1, &mInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(mLogicalDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        mShouldResize = false;
        return;
    } else
    {
        PK_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "vkAcquireNextImageKHR failed to acquire swap chain image.");
    }
    vkResetCommandBuffer(mCommandBuffers[currentFrame], 0);
    RecordCommandBuffer(mCommandBuffers[currentFrame], imageIndex);

    vkResetFences(mLogicalDevice, 1, &mInFlightFences[currentFrame]);

    UpdateUniformBuffer(currentFrame);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[currentFrame];
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[currentFrame];
    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[currentFrame]);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &mSwapChain;
    presentInfo.pImageIndices = &imageIndex;
    vkQueuePresentKHR(mGraphicsQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mShouldResize)
    {
        RecreateSwapChain();
        mShouldResize = false;
    } else
    {
        PK_ASSERT(result == VK_SUCCESS, "vkQueueSubmit failed to submit command buffer.");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanContext::ResizeViewport()
{
    mShouldResize = true;
}

void VulkanContext::WaitIdle()
{
    vkDeviceWaitIdle(mLogicalDevice);
}

void VulkanContext::CreateInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Piksela";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "Piksela";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Query validation layers
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    PK_ASSERT(CheckValidationLayerSupport(validationLayers), "Requested Validation Layers are not supported.");

    // Query instance extensions
    std::vector<const char *> requiredExtensions = GetRequiredExtensions();

    VkInstanceCreateInfo instCreateInfo = {};
    instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instCreateInfo.pApplicationInfo = &appInfo;

    instCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    instCreateInfo.ppEnabledLayerNames = validationLayers.data();
    instCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    instCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = VulkanContext::DebugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;

    instCreateInfo.pNext = &debugMessengerCreateInfo;

    VkResult result = vkCreateInstance(&instCreateInfo, nullptr, &mInstance);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateInstance failed to create a Vulkan Instance.");

    volkLoadInstance(mInstance);
}

bool VulkanContext::CheckValidationLayerSupport(const std::vector<const char *> &validationLayers)
{
    // Query available layers
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check if requested validation layers are available
    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanContext::GetRequiredExtensions()
{
    uint32_t extensionCount = 0;
    const char **glfwExtensions;
#ifdef PK_WIN32
    glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
#endif

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + extensionCount);
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void VulkanContext::CreateDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
    debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerCreateInfo.pfnUserCallback = VulkanContext::DebugCallback;
    debugMessengerCreateInfo.pUserData = nullptr;

    VkResult result = vkCreateDebugUtilsMessengerEXT(mInstance, &debugMessengerCreateInfo, nullptr, &mDebugMessenger);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateDebugUtilsMessengerEXT failed to create Debug Messenger.");
}

void VulkanContext::CreateSurface()
{
#ifdef PK_GLFW
    VkResult result = glfwCreateWindowSurface(mInstance, (GLFWwindow *)mWindow->GetNativeWindow(), nullptr, &mSurface);
    PK_ASSERT(result == VK_SUCCESS, "glfwCreateWindowSurface failed to create window surface.");
#endif
}

void VulkanContext::CreatePhysicalDevice()
{
    // Query available devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    PK_ASSERT(deviceCount != 0, "vkEnumeratePhysicalDevices found 0 available devices.");

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, physicalDevices.data());

    int32_t deviceRank = -1;
    VkPhysicalDevice bestDevice;
    CoreLogger::Trace("Vulkan Devices:");
    for (const VkPhysicalDevice &device : physicalDevices)
    {
        VkPhysicalDeviceProperties deviceProps;
        vkGetPhysicalDeviceProperties(device, &deviceProps);

        bool presentSupported = false;

        bool swapChainSupported = false;
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensionProps(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensionProps.data());
        for (const VkExtensionProperties &extensionProp : extensionProps)
        {
            if (strcmp(extensionProp.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
            {
                uint32_t formatCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

                if (formatCount != 0 && presentModeCount != 0)
                {
                    swapChainSupported = true;
                } else
                {
                    swapChainSupported = false;
                }
                break;
            }
        }

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueProps(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueProps.data());
        for (int i = 0; i < queueProps.size(); i++)
        {
            VkBool32 support;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &support);
            if (swapChainSupported && support && (queueProps[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
            {
                presentSupported = true;
            }
        }

        std::string name = deviceProps.deviceName;
        std::string deviceType;
        switch (deviceProps.deviceType)
        {
            case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            {
                deviceType = "Discrete";
                break;
            }
            case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            {
                deviceType = "Integrated";
                break;
            }
            case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            {
                deviceType = "Virtual";
                break;
            }
            case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
            {
                deviceType = "CPU";
                break;
            }
            default:
            {
                deviceType = "Other";
                break;
            }
        }
        CoreLogger::Trace(" # GPU Name: {0} - Supports Presentation: {1} - Device Type: {2}", name, presentSupported ? "True" : "False", deviceType);

        if (presentSupported)
        {
            switch (deviceProps.deviceType)
            {
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                {
                    if (deviceRank < 4)
                    {
                        deviceRank = 3;
                        bestDevice = device;
                    }
                    break;
                }
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                {
                    if (deviceRank < 3)
                    {
                        deviceRank = 2;
                        bestDevice = device;
                    }
                    break;
                }
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                {
                    if (deviceRank < 2)
                    {
                        deviceRank = 1;
                        bestDevice = device;
                    }
                    break;
                }
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
                {
                    if (deviceRank < 1)
                    {
                        deviceRank = 0;
                        bestDevice = device;
                    }
                    break;
                }
                default:
                {
                    if (deviceRank < 0)
                    {
                        deviceRank = 0;
                        bestDevice = device;
                    }
                }
            }
        }
    }
    PK_ASSERT(deviceRank != -1, "Could not find a device that supports graphics and present queues.");
    mPhysicalDevice = bestDevice;
}

void VulkanContext::CreateLogicalDevice()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueProps.data());

    mGraphicsQueueFamilyIndex = UINT32_MAX;
    mPresentQueueFamilyIndex = UINT32_MAX;
    for (int i = 0; i < queueProps.size(); i++)
    {
        if (queueProps[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
        {
            if (mGraphicsQueueFamilyIndex == UINT32_MAX)
            {
                mGraphicsQueueFamilyIndex = i;
            }

            VkBool32 support;
            vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &support);
            if (support == VK_TRUE)
            {
                mGraphicsQueueFamilyIndex = i;
                mPresentQueueFamilyIndex = i;
            }
        }
    }

    // Prefer a Queue that supports both graphics and present
    // If not available, find another present queue
    if (mPresentQueueFamilyIndex == UINT_MAX)
    {
        for (int i = 0; i < queueProps.size(); i++)
        {
            if (queueProps[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
            {
                VkBool32 support;
                vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &support);
                if (support == VK_TRUE)
                {
                    mPresentQueueFamilyIndex = i;
                }
            }
        }
    }

    bool seperateQueues = (mGraphicsQueueFamilyIndex != mPresentQueueFamilyIndex);
    std::vector<VkDeviceQueueCreateInfo> dqCreateInfos;
    float queuePriority = 1.0f;
    if (seperateQueues)
    {
        dqCreateInfos.resize(2);
        dqCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dqCreateInfos[0].queueFamilyIndex = mGraphicsQueueFamilyIndex;
        dqCreateInfos[0].pQueuePriorities = &queuePriority;
        dqCreateInfos[0].queueCount = 1;

        dqCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dqCreateInfos[1].queueFamilyIndex = mPresentQueueFamilyIndex;
        dqCreateInfos[1].pQueuePriorities = &queuePriority;
        dqCreateInfos[1].queueCount = 1;
    } else
    {
        dqCreateInfos.resize(1);
        dqCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dqCreateInfos[0].queueFamilyIndex = mGraphicsQueueFamilyIndex;
        dqCreateInfos[0].pQueuePriorities = &queuePriority;
        dqCreateInfos[0].queueCount = 1;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo dCreateInfo = {};
    dCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dCreateInfo.pQueueCreateInfos = dqCreateInfos.data();
    dCreateInfo.queueCreateInfoCount = dqCreateInfos.size();
    dCreateInfo.pEnabledFeatures = &deviceFeatures;
    dCreateInfo.enabledExtensionCount = deviceExtensions.size();
    dCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    dCreateInfo.enabledLayerCount = 0;

    VkResult result = vkCreateDevice(mPhysicalDevice, &dCreateInfo, nullptr, &mLogicalDevice);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateDevice failed to create logical device.");

    vkGetDeviceQueue(mLogicalDevice, mGraphicsQueueFamilyIndex, 0, &mGraphicsQueue);

    if (!seperateQueues)
    {
        mPresentQueue = mGraphicsQueue;
    } else
    {
        vkGetDeviceQueue(mLogicalDevice, mPresentQueueFamilyIndex, 0, &mPresentQueue);
    }
}

void VulkanContext::RecreateSwapChain()
{
    CoreLogger::Info("Recreating Swapchain.");
    // TODO: Recreate swap chain while drawing commands
    // on the old swapchain are still in-flight
    vkDeviceWaitIdle(mLogicalDevice);

    CleanupSwapChain();

    CreateSwapChain();
    CreateFramebuffers();
}

void VulkanContext::CreateSwapChain()
{
    // Get the list of formats that are supported
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, formats.data());

    bool foundFormat = false;
    VkSurfaceFormatKHR selectedFormat;
    for (const VkSurfaceFormatKHR &format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM)
        {
            selectedFormat = format;
            foundFormat = true;
            break;
        }
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM)
        {
            selectedFormat = format;
            foundFormat = true;
        }
    }
    PK_ASSERT(foundFormat == true, "Failed to find suitable surface format.");

    // Get the list of present modes that are supported
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mSurface, &presentModeCount, presentModes.data());

    VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const VkPresentModeKHR &presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            selectedPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }

    // Get the surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &capabilities);

    VkExtent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX && capabilities.currentExtent.height != UINT32_MAX)
    {
        extent = capabilities.currentExtent;
        CoreLogger::Info("[{0}, {1}]", extent.width, extent.height);
    } else
    {
        uint32_t width = mWindow->GetWidth();
        uint32_t height = mWindow->GetHeight();
        CoreLogger::Info("{0}, {1}", width, height);

        // TODO: Replace with Math::clamp
        if (width >= capabilities.minImageExtent.width && width <= capabilities.maxImageExtent.width)
        {
            extent.width = width;
        } else
        {
            extent.width = (width < capabilities.minImageExtent.width) ? capabilities.minImageExtent.width : capabilities.maxImageExtent.width;
        }

        if (height >= capabilities.minImageExtent.height && height <= capabilities.maxImageExtent.height)
        {
            extent.height = height;
        } else
        {
            extent.height = (height < capabilities.minImageExtent.height) ? capabilities.minImageExtent.height : capabilities.maxImageExtent.height;
        }
    }
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.minImageCount != 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.minImageCount;
    }

    uint32_t queueFamilyIndices[] = {
        mGraphicsQueueFamilyIndex,
        mPresentQueueFamilyIndex
    };
    VkSwapchainCreateInfoKHR scCreateInfo = {};
    scCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    scCreateInfo.surface = mSurface;
    scCreateInfo.minImageCount = imageCount;
    scCreateInfo.imageFormat = selectedFormat.format;
    scCreateInfo.imageColorSpace = selectedFormat.colorSpace;
    scCreateInfo.imageExtent = extent;
    scCreateInfo.imageArrayLayers = 1;
    scCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (mGraphicsQueueFamilyIndex != mPresentQueueFamilyIndex)
    {
        scCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        scCreateInfo.queueFamilyIndexCount = 2;
        scCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else
    {
        scCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    scCreateInfo.preTransform = capabilities.currentTransform;
    scCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    scCreateInfo.presentMode = selectedPresentMode;
    scCreateInfo.clipped = VK_TRUE;
    scCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    // Create Vulkan swap chain
    VkResult result = vkCreateSwapchainKHR(mLogicalDevice, &scCreateInfo, nullptr, &mSwapChain);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateSwapchainKHR failed to create swap chain.");

    uint32_t actualImageCount;
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &actualImageCount, nullptr);
    mSwapChainImages.resize(actualImageCount);
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &actualImageCount, mSwapChainImages.data());

    // Create swap chain image views
    mSwapChainFormat = selectedFormat.format;
    mSwapChainExtent = extent;
    mSwapChainImageViews.resize(mSwapChainImages.size());
    for (int i = 0; i < actualImageCount; i++)
    {
        VkImageViewCreateInfo ivCreateInfo = {};
        ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivCreateInfo.image = mSwapChainImages[i];
        ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivCreateInfo.format = mSwapChainFormat;
        ivCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ivCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivCreateInfo.subresourceRange.baseMipLevel = 0;
        ivCreateInfo.subresourceRange.levelCount = 1;
        ivCreateInfo.subresourceRange.baseArrayLayer = 0;
        ivCreateInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(mLogicalDevice, &ivCreateInfo, nullptr, &mSwapChainImageViews[i]);
        PK_ASSERT(result == VK_SUCCESS, "vkCreateImageView failed to create image view #{0}.", i);
    }
}

void VulkanContext::CleanupSwapChain()
{
    for (const VkFramebuffer &framebuffer : mFramebuffers)
    {
        vkDestroyFramebuffer(mLogicalDevice, framebuffer, nullptr);
    }
    for (const VkImageView &imageView : mSwapChainImageViews)
    {
        vkDestroyImageView(mLogicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mLogicalDevice, mSwapChain, nullptr);
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription colorDescription = {};
    colorDescription.format = mSwapChainFormat;
    colorDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    colorDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;

    VkRenderPassCreateInfo rpCreateInfo = {};
    rpCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpCreateInfo.attachmentCount = 1;
    rpCreateInfo.pAttachments = &colorDescription;
    rpCreateInfo.subpassCount = 1;
    rpCreateInfo.pSubpasses = &subpassDescription;

    VkResult result = vkCreateRenderPass(mLogicalDevice, &rpCreateInfo, nullptr, &mRenderPass);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateRenderPass failed to create render pass.");
}

void VulkanContext::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding dsLayoutBinding = {};
    dsLayoutBinding.binding = 0;
    dsLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dsLayoutBinding.descriptorCount = 1;
    dsLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo dslCreateInfo = {};
    dslCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dslCreateInfo.bindingCount = 1;
    dslCreateInfo.pBindings = &dsLayoutBinding;

    VkResult result = vkCreateDescriptorSetLayout(mLogicalDevice, &dslCreateInfo, nullptr, &mDescriptorSetLayout);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateDescriptorSetLayout failed to create layout.");
}

void VulkanContext::CreateGraphicsPipeline()
{
    // Shader Modules
    std::string vertexSource = ReadSpirVFile("C:/dev/Piksela/Assets/Shaders/StandardVert.spv");
    std::string fragmentSource = ReadSpirVFile("C:/dev/Piksela/Assets/Shaders/StandardFrag.spv");

    VkShaderModule vertexShaderModule = CreateShaderModule(vertexSource);
    VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentSource);

    VkPipelineShaderStageCreateInfo vssCreateInfo = {};
    vssCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vssCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vssCreateInfo.module = vertexShaderModule;
    vssCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fssCreateInfo = {};
    fssCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fssCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fssCreateInfo.module = fragmentShaderModule;
    fssCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo pssCreateInfos[] = { vssCreateInfo, fssCreateInfo };

    // Fixed-function operations
    VkPipelineViewportStateCreateInfo vsCreateInfo = {};
    vsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vsCreateInfo.viewportCount = 1;
    vsCreateInfo.scissorCount = 1;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dsCreateInfo = {};
    dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dsCreateInfo.dynamicStateCount = dynamicStates.size();
    dsCreateInfo.pDynamicStates = dynamicStates.data();

    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = Vertex::GetAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo visCreateInfo = {};
    visCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    visCreateInfo.vertexBindingDescriptionCount = 1;
    visCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    visCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    visCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo iasCreateInfo = {};
    iasCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iasCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    iasCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rsCreateInfo = {};
    rsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rsCreateInfo.depthClampEnable = VK_FALSE;
    rsCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rsCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rsCreateInfo.lineWidth = 1.0f;
    rsCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rsCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rsCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo msCreateInfo = {};
    msCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msCreateInfo.sampleShadingEnable = VK_FALSE;
    msCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState cbaState = {};
    cbaState.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    cbaState.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo cbsCreateInfo = {};
    cbsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cbsCreateInfo.logicOpEnable = VK_FALSE;
    cbsCreateInfo.attachmentCount = 1;
    cbsCreateInfo.pAttachments = &cbaState;

    VkPipelineLayoutCreateInfo plCreateInfo = {};
    plCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plCreateInfo.setLayoutCount = 1;
    plCreateInfo.pSetLayouts = &mDescriptorSetLayout;
    VkResult result = vkCreatePipelineLayout(mLogicalDevice, &plCreateInfo, nullptr, &mPipelineLayout);
    PK_ASSERT(result == VK_SUCCESS, "vkCreatePipelineLayout failed to create pipeline layout.");

    VkGraphicsPipelineCreateInfo gpCreateInfo = {};
    gpCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gpCreateInfo.stageCount = 2;
    gpCreateInfo.pStages = pssCreateInfos;
    gpCreateInfo.pVertexInputState = &visCreateInfo;
    gpCreateInfo.pInputAssemblyState = &iasCreateInfo;
    gpCreateInfo.pViewportState = &vsCreateInfo;
    gpCreateInfo.pRasterizationState = &rsCreateInfo;
    gpCreateInfo.pMultisampleState = &msCreateInfo;
    gpCreateInfo.pColorBlendState = &cbsCreateInfo;
    gpCreateInfo.pDynamicState = &dsCreateInfo;
    gpCreateInfo.layout = mPipelineLayout;
    gpCreateInfo.renderPass = mRenderPass;
    gpCreateInfo.subpass = 0;

    result = vkCreateGraphicsPipelines(mLogicalDevice, VK_NULL_HANDLE, 1, &gpCreateInfo, nullptr, &mGraphicsPipeline);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateGraphicsPipelines failed to create {0} graphics pipeline(s).", 1);

    vkDestroyShaderModule(mLogicalDevice, vertexShaderModule, nullptr);
    vkDestroyShaderModule(mLogicalDevice, fragmentShaderModule, nullptr);
}

std::string VulkanContext::ReadSpirVFile(const std::string &path)
{
    std::string source;
    std::ifstream in(path, std::ios::ate | std::ios::binary);
    if (!in.is_open())
    {
        // TODO: instead send error message and somehow
        // handle loading default shader
        PK_ASSERT(false, "Failed to open Spir-V file '{0}'.", path);
    }
    source.resize(in.tellg());
    in.seekg(0);
    in.read(&source[0], source.size());
    in.close();

    CoreLogger::Trace("Loaded file '{0}' of size {1} bytes.", path, source.size());
    return source;
}

VkShaderModule VulkanContext::CreateShaderModule(const std::string &source)
{
    VkShaderModuleCreateInfo smCreateInfo = {};
    smCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smCreateInfo.codeSize = source.size();
    smCreateInfo.pCode = reinterpret_cast<const uint32_t *>(source.c_str());

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(mLogicalDevice, &smCreateInfo, nullptr, &shaderModule);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateShaderModule failed to create shader module.");
    return shaderModule;
}

void VulkanContext::CreateFramebuffers()
{
    mFramebuffers.resize(mSwapChainImageViews.size());

    for (int i = 0; i < mSwapChainImageViews.size(); i++)
    {
        VkImageView *attachments = &mSwapChainImageViews[i];

        VkFramebufferCreateInfo fCreateInfo = {};
        fCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fCreateInfo.renderPass = mRenderPass;
        fCreateInfo.attachmentCount = sizeof(attachments) / sizeof(VkImageView *);
        fCreateInfo.pAttachments = attachments;
        fCreateInfo.width = mSwapChainExtent.width;
        fCreateInfo.height = mSwapChainExtent.height;
        fCreateInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mLogicalDevice, &fCreateInfo, nullptr, &mFramebuffers[i]);
        PK_ASSERT(result == VK_SUCCESS, "vkCreateFramebuffer failed to create framebuffer #{0}.", i);
    }
}

void VulkanContext::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(uint16_t) * mIndices.size();
    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags stagingBufferProps = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, stagingBufferUsage, stagingBufferProps, stagingBuffer, stagingBufferMemory);

    void *data;
    vkMapMemory(mLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mIndices.data(), bufferSize);
    vkUnmapMemory(mLogicalDevice, stagingBufferMemory);

    VkBufferUsageFlags indexBufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    CreateBuffer(bufferSize, indexBufferUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);
    CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);
    vkDestroyBuffer(mLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(mLogicalDevice, stagingBufferMemory, nullptr);
}

void VulkanContext::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(ShaderMatrices);
    mUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mUniformBufferMemories.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkMemoryPropertyFlags uniformBufferProps = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformBufferProps, mUniformBuffers[i], mUniformBufferMemories[i]);
    }
}

void VulkanContext::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    // Create Buffer
    VkBufferCreateInfo bCreateInfo = {};
    bCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bCreateInfo.size = size;
    bCreateInfo.usage = usage;
    bCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(mLogicalDevice, &bCreateInfo, nullptr, &buffer);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateBuffer failed to create buffer.");

    // Allocate Memory
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(mLogicalDevice, buffer, &requirements);
    uint32_t index = FindMemoryType(requirements.memoryTypeBits, props);

    VkMemoryAllocateInfo mAllocateInfo = {};
    mAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mAllocateInfo.allocationSize = requirements.size;
    mAllocateInfo.memoryTypeIndex = index;

    result = vkAllocateMemory(mLogicalDevice, &mAllocateInfo, nullptr, &bufferMemory);
    PK_ASSERT(result == VK_SUCCESS, "vkAllocateMemory failed to allocate memory for the buffer.");

    vkBindBufferMemory(mLogicalDevice, buffer, bufferMemory, 0);
}

void VulkanContext::CopyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize bufferSize)
{
    VkCommandBufferAllocateInfo cbAllocateInfo = {};
    cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocateInfo.commandPool = mCommandPool;
    cbAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mLogicalDevice, &cbAllocateInfo, &commandBuffer);
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
    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mGraphicsQueue);

    vkFreeCommandBuffers(mLogicalDevice, mCommandPool, 1, &commandBuffer);
}

void VulkanContext::UpdateUniformBuffer(uint32_t currentFrame)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ShaderMatrices matrices = {};
    matrices.Model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    matrices.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    matrices.Projection = glm::perspective(glm::radians(45.0f), mSwapChainExtent.width / (float)mSwapChainExtent.height, 0.1f, 10.0f);
    matrices.Projection[1][1] *= -1;

    void *data;
    vkMapMemory(mLogicalDevice, mUniformBufferMemories[currentFrame], 0, sizeof(ShaderMatrices), 0, &data);
    memcpy(data, &matrices, sizeof(ShaderMatrices));
    vkUnmapMemory(mLogicalDevice, mUniformBufferMemories[currentFrame]);
}

void VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo cpCreateInfo = {};
    cpCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cpCreateInfo.queueFamilyIndex = mGraphicsQueueFamilyIndex;

    VkResult result = vkCreateCommandPool(mLogicalDevice, &cpCreateInfo, nullptr, &mCommandPool);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateCommandPool failed to create command pool.");
}

void VulkanContext::CreateCommandBuffers()
{
    mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo cbAllocInfo = {};
    cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbAllocInfo.commandPool = mCommandPool;
    cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbAllocInfo.commandBufferCount = mCommandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(mLogicalDevice, &cbAllocInfo, mCommandBuffers.data());
    PK_ASSERT(result == VK_SUCCESS, "vkAllocateCommandBuffers failed to allocate command buffers.");
}

void VulkanContext::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    // Begin Command Buffer
    VkCommandBufferBeginInfo cbBeginInfo = {};
    cbBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkResult result = vkBeginCommandBuffer(commandBuffer, &cbBeginInfo);
    PK_ASSERT(result == VK_SUCCESS, "vkBeginCommandBuffer failed to begin command buffer.");

    // Record Commands
    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = mRenderPass;
    rpBeginInfo.framebuffer = mFramebuffers[imageIndex];
    rpBeginInfo.renderArea.offset = { 0, 0 };
    rpBeginInfo.renderArea.extent = mSwapChainExtent;
    VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(commandBuffer, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)mSwapChainExtent.width;
    viewport.height = (float)mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = mSwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer, &offset);
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[imageIndex], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, mIndices.size(), 1, 0, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    // End Command Buffer
    result = vkEndCommandBuffer(commandBuffer);
    PK_ASSERT(result == VK_SUCCESS, "vkEndCommandBuffer failed to end command buffer.");
}

void VulkanContext::CreateSyncObjects()
{
    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo sCreateInfo = {};
    sCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fCreateInfo = {};
    fCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkResult result = vkCreateSemaphore(mLogicalDevice, &sCreateInfo, nullptr, &mImageAvailableSemaphores[i]);
        PK_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed to create mImageAvailableSemaphore.");
        result = vkCreateSemaphore(mLogicalDevice, &sCreateInfo, nullptr, &mRenderFinishedSemaphores[i]);
        PK_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed to create mRenderFinishedSemaphore.");
        result = vkCreateFence(mLogicalDevice, &fCreateInfo, nullptr, &mInFlightFences[i]);
        PK_ASSERT(result == VK_SUCCESS, "vkCreateSemaphore failed to create mInFlightFence.");
    }
}

uint32_t VulkanContext::FindMemoryType(uint32_t filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &props);
    for (uint32_t i = 0; i < props.memoryTypeCount; i++)
    {
        bool matchesFilter = filter & (1 << i);
        bool matchesProps = (props.memoryTypes[i].propertyFlags & properties) == properties;
        if (matchesFilter && matchesProps)
        {
            return i;
        }
    }

    PK_ASSERT(false, "Failed to find suitable memory type.");
    return -1;
}

void VulkanContext::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo dpCreateInfo = {};
    dpCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpCreateInfo.poolSizeCount = 1;
    dpCreateInfo.pPoolSizes = &poolSize;
    dpCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

    VkResult result = vkCreateDescriptorPool(mLogicalDevice, &dpCreateInfo, nullptr, &mDescriptorPool);
    PK_ASSERT(result == VK_SUCCESS, "vkCreateDescriptorPool failed to create descriptor pool.");
}

void VulkanContext::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo dsAllocInfo = {};
    dsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsAllocInfo.descriptorPool = mDescriptorPool;
    dsAllocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
    dsAllocInfo.pSetLayouts = layouts.data();

    mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(mLogicalDevice, &dsAllocInfo, mDescriptorSets.data());
    PK_ASSERT(result == VK_SUCCESS, "vkAllocateDescriptorSets failed to allocate descriptor sets.");

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = mUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(ShaderMatrices);

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mDescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(mLogicalDevice, 1, &descriptorWrite, 0, nullptr);
    }
}

} // namespace Piksela