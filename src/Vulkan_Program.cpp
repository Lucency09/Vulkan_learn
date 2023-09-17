#include "Vulkan_Program.h"


Vulkan_program::Vulkan_program()
{
    return;
}

void Vulkan_program::Init()
{
    toy2d::Context::Init();
}

void Vulkan_program::Quit()
{
    toy2d::Context::Quit();
}

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
    this->Init();
    this->instance = toy2d::Context::GetInstance().get_instance();
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
    this->Quit();

    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void check_Extension()
{
    //检查被支持的扩展数量
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //获取被支持的扩展名称
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    //打印被支持的扩展
    std::cout << "available extensions:\n";
    for (const VkExtensionProperties& extension : extensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

bool checkValidationLayerSupport() 
{
    //检查被支持的扩展数量
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    //获取被支持的扩展名称
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    //判断需要支持的扩展validationLayers是否都在被支持的扩展availableLayers中
    for (const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers)
        {//遍历支持的扩展
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