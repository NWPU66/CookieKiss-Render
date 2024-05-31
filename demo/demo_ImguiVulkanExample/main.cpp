#include "stdInclude.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

//Data
static VkAllocationCallbacks* g_Allocator      = nullptr;
static VkInstance             g_Instance       = VK_NULL_HANDLE;
static VkPhysicalDevice       g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice               g_Device         = VK_NULL_HANDLE;
// static uint32_t                 g_QueueFamily    = (uint32_t)-1;
static uint32_t                 g_QueueFamily    = std::numeric_limits<uint32_t>::max();
static VkQueue                  g_Queue          = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport    = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache  = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount    = 2;
static bool                     g_SwapChainRebuild = false;

//Function
static void check_vk_result(VkResult err);
static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties,
                                 const char*                               extension);
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT      flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t                   object,
                                                   size_t                     location,
                                                   int32_t                    messageCode,
                                                   const char*                pLayerPrefix,
                                                   const char*                pMessage,
                                                   void*                      pUserData);
static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();
static void             SetupVulkan(std::vector<const char*>& instance_extensions);
static void             SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd,
                                          VkSurfaceKHR              surface,
                                          int                       width,
                                          int                       height);
static ImGuiIO& SetupImgui(GLFWwindow* window);

int main(int, char**)
{
    //初始化GLFW窗口
    if (!glfwInit()) { return EXIT_FAILURE; }
    //Create window with Vulkan context
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 960, "Vulkan", nullptr, nullptr);
    if (!glfwVulkanSupported())
    {
        printf("GLFW: Vulkan Not Supported\n");
        return EXIT_FAILURE;
    }
    //获取GLFW扩展
    std::vector<const char*> extensions;
    uint32_t                 extensions_count = 0;
    const char**             glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
    {
        extensions.push_back(*(glfw_extensions + i));
    }
    SetupVulkan(extensions);

    //创建窗口界面
    VkSurfaceKHR surface;
    VkResult     err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
    check_vk_result(err);

    //创建帧缓冲
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
    SetupVulkanWindow(wd, surface, w, h);

    //初始化Imgui
    SetupImgui();

    return EXIT_SUCCESS;
}

static void check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS) { return; }
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0) { abort(); }
}

static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties,
                                 const char*                               extension)
{
    for (const auto& p : properties)
    {
        if (strcmp(p.extensionName, extension) == 0) { return true; }
    }
    return false;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT      flags,
                                                   VkDebugReportObjectTypeEXT objectType,
                                                   uint64_t                   object,
                                                   size_t                     location,
                                                   int32_t                    messageCode,
                                                   const char*                pLayerPrefix,
                                                   const char*                pMessage,
                                                   void*                      pUserData)
{
    (void)flags;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    (void)pLayerPrefix; // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType,
            pMessage);
    return VK_FALSE;
}

static VkPhysicalDevice SetupVulkan_SelectPhysicalDevice()
{
    uint32_t gpu_count;
    VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
    check_vk_result(err);
    assert(gpu_count>0);

    std::vector<VkPhysicalDevice> gpus;
    gpus.resize(gpu_count);
    err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data());
    check_vk_result(err);

    /**NOTE - GPU select rule:
     * If a number >1 of GPUs got reported, find discrete GPU if present,
     * or use first one available.
     */
    for (const auto& device : gpus)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) { return device; }
    }

    // Use first GPU (Integrated) is a Discrete one is not available.
    if (gpu_count > 0) { return gpus[0]; }
    return VK_NULL_HANDLE;
}

static void SetupVulkan(std::vector<const char*>& instance_extensions)
{
    VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkInitialize();
#endif

    //1. create vkInstance
    {
        VkInstanceCreateInfo create_info = {};
        create_info.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        //枚举可用的扩展
        uint32_t                           properties_count;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count,
                                                     properties.data());
        check_vk_result(err);

        //启动需要的扩展
        if (IsExtensionAvailable(properties,
                                 VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
        {
            instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif

        //启动验证层
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        const char* layers[]            = {"VK_LAYER_KHRONOS_validation"};
        create_info.enabledLayerCount   = 1;
        create_info.ppEnabledLayerNames = layers;
        instance_extensions.push_back("VK_EXT_debug_report");
#endif

        //创建！！！
        create_info.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size());
        create_info.ppEnabledExtensionNames = instance_extensions.data();
        err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
        check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
        volkLoadInstance(g_Instance);
#endif

        //创建debug回调函数
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)
            vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
        assert(f_vkCreateDebugReportCallbackEXT != nullptr,
               "vkGetInstanceProcAddr failed to find vkCreateDebugReportCallbackEXT");
        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData = nullptr;
        err = f_vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator,
                                               &g_DebugReport);
        check_vk_result(err);
#endif
    }

    //2.检查物理设备，并选择队列族
    {
        g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

        //选择队列族
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(
            sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
        for (uint32_t i = 0; i < count; i++)
        {
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                g_QueueFamily = i;
                break;
            }
        }
        free(queues); //记得释放堆上的内存
        assert(g_QueueFamily!=0xFFFFFFFF);
    }

    //3.创建逻辑设备 (with 1 queue)
    {
        std::vector<const char*> device_extensions = {"VK_KHR_swapchain"};

        //列出物理设备的扩展
        uint32_t                           properties_count;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count,
                                             properties.data());
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        const float             queue_priority = 1;
        VkDeviceQueueCreateInfo queue_info[1]  = {};
        queue_info[0].sType                    = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex         = g_QueueFamily;
        queue_info[0].queueCount               = 1;
        queue_info[0].pQueuePriorities         = &queue_priority;

        VkDeviceCreateInfo create_info      = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount    = IM_ARRAYSIZE(queue_info);
        create_info.pQueueCreateInfos       = queue_info;
        create_info.enabledExtensionCount   = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
        check_vk_result(err);
        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    //4. Create Descriptor Pool
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1;
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
        check_vk_result(err);
    }
}

static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd,
                              VkSurfaceKHR              surface,
                              int                       width,
                              int                       height) {}

static ImGuiIO& SetupImgui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                  = g_Instance;
    init_info.PhysicalDevice            = g_PhysicalDevice;
    init_info.Device                    = g_Device;
    init_info.QueueFamily               = g_QueueFamily;
    init_info.Queue                     = g_Queue;
    init_info.PipelineCache             = g_PipelineCache;
    init_info.DescriptorPool            = g_DescriptorPool;
    init_info.RenderPass                = wd->RenderPass;
    init_info.Subpass                   = 0;
    init_info.MinImageCount             = g_MinImageCount;
    init_info.ImageCount                = wd->ImageCount;
    init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator                 = g_Allocator;
    init_info.CheckVkResultFn           = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);
    //TODO

    return io;
}
