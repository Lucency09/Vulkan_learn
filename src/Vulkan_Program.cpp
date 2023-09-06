#include "Vulkan_Program.h"

void Vulkan_program::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Vulkan_program::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window = glfwCreateWindow(this->WIDTH, this->HEIGHT, "Vulkan", nullptr, nullptr);
}

void Vulkan_program::initVulkan()
{
    createInstance();
}

void Vulkan_program::mainLoop()
{
    while (!glfwWindowShouldClose(this->window))
    {
        glfwPollEvents();
    }
}

void Vulkan_program::cleanup()
{
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Vulkan_program::createInstance()
{
    //设置应用程序信息，例如名称、版本
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//枚举，说明结构体类型
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //设置Vulkan实例的创建参数
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // 枚举，说明结构体类型
    createInfo.pApplicationInfo = &appInfo;//此处使用上诉创建的VkApplicationInfo变量

    //使用glfw库指定与Windows交互的扩展
    uint32_t glfwExtensionCount = 0;//用于存储与windows扩展的个数
    const char** glfwExtensions;//用于存储与windows扩展的名称列表
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);//调用glfw获取扩展以修改glfwExtensionCount和glfwExtensions
    createInfo.enabledExtensionCount = glfwExtensionCount;//设置扩展个数
    createInfo.ppEnabledExtensionNames = glfwExtensions;//设置扩展名称

    createInfo.enabledLayerCount = 0;//指定全局校验层

    VkResult result = vkCreateInstance(&createInfo,nullptr, &this->instance);//创建vulkan实例

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void Vulkan_program::check_Extension()
{
    //检查被支持的扩展数量
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //获取被支持的扩展名称
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    //打印被支持的扩展
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}