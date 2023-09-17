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
    //��鱻֧�ֵ���չ����
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //��ȡ��֧�ֵ���չ����
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    //��ӡ��֧�ֵ���չ
    std::cout << "available extensions:\n";
    for (const VkExtensionProperties& extension : extensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

bool checkValidationLayerSupport() 
{
    //��鱻֧�ֵ���չ����
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    //��ȡ��֧�ֵ���չ����
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    //�ж���Ҫ֧�ֵ���չvalidationLayers�Ƿ��ڱ�֧�ֵ���չavailableLayers��
    for (const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers)
        {//����֧�ֵ���չ
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