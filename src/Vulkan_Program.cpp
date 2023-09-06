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
    //����Ӧ�ó�����Ϣ���������ơ��汾
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;//ö�٣�˵���ṹ������
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //����Vulkanʵ���Ĵ�������
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; // ö�٣�˵���ṹ������
    createInfo.pApplicationInfo = &appInfo;//�˴�ʹ�����ߴ�����VkApplicationInfo����

    //ʹ��glfw��ָ����Windows��������չ
    uint32_t glfwExtensionCount = 0;//���ڴ洢��windows��չ�ĸ���
    const char** glfwExtensions;//���ڴ洢��windows��չ�������б�
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);//����glfw��ȡ��չ���޸�glfwExtensionCount��glfwExtensions
    createInfo.enabledExtensionCount = glfwExtensionCount;//������չ����
    createInfo.ppEnabledExtensionNames = glfwExtensions;//������չ����

    createInfo.enabledLayerCount = 0;//ָ��ȫ��У���

    VkResult result = vkCreateInstance(&createInfo,nullptr, &this->instance);//����vulkanʵ��

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void Vulkan_program::check_Extension()
{
    //��鱻֧�ֵ���չ����
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    //��ȡ��֧�ֵ���չ����
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    //��ӡ��֧�ֵ���չ
    std::cout << "available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}