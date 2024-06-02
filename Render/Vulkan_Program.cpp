#include "Render/include/Vulkan_Program.h"


Vulkan_program::Vulkan_program()
{
    return;
}

Vulkan_program::~Vulkan_program()
{
    //this->cleanup();
}

void Vulkan_program::Init()
{
    check_glfwExtension();//��ʼ��glfwExtensions_vec
    toy2d::Context::Init(glfwExtensions_vec,
        [&](vk::Instance instance) {
            VkSurfaceKHR surface;
            if (glfwCreateWindowSurface(instance, window, NULL,&surface)) 
            { //vulkan���;glfw����;�ڴ������(NULLΪĬ�Ϸ�����);���ڴ洢vulkan���ڱ���
                throw std::runtime_error("can't create surface");
            }
            return surface;
        });

    toy2d::Context::GetInstance().InitSwapchain(toy2d::WINDOWS_WIDTH, toy2d::WINDOWS_HIGHT);

    toy2d::Context::GetInstance().InitRender_process("res/Spir-v/Vertex01.spv", "res/Spir-v/Fragment01.spv", toy2d::WINDOWS_HIGHT, toy2d::WINDOWS_WIDTH);

    toy2d::Context::GetInstance().InitRenderer();
}

void Vulkan_program::Quit()
{
    //toy2d::Context::GetInstance().DestroySwapchain();
    //toy2d::Context::Quit();
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

    this->window = glfwCreateWindow(toy2d::WINDOWS_WIDTH, toy2d::WINDOWS_HIGHT, "Vulkan", nullptr, nullptr);
}

void Vulkan_program::initVulkan()
{
    this->Init();
    this->instance = toy2d::Context::GetInstance().get_instance();
}

void Vulkan_program::mainLoop()
{
    auto& render = toy2d::Context::GetInstance().get_renderer();
    toy2d::Camera camera;

    while (!glfwWindowShouldClose(this->window))
    {
        glfwPollEvents();

        camera.set_M(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f)));
        render.set_UniformMVP(camera.get_MVP());//����uniformMVP

        render.StartRender();
        render.Draw();//����
        render.EndRender();

        //render.random_rotation();//�����ת
        //Sleep(5);//��ʱ

        // ��ȡ�û�����
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            // �û�������ESC�����˳�����
            glfwSetWindowShouldClose(this->window, GLFW_TRUE);
        }
    }
}

void Vulkan_program::cleanup()
{
    
    this->Quit();
    glfwDestroyWindow(this->window);
    glfwTerminate();
    
}

void Vulkan_program::check_glfwExtension()
{
    uint32_t glfwExtensionCount = 0; //��չ����
    const char** glfwExtensions;// ������չ
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);//������������ֵ
    glfwExtensions_vec.reserve(glfwExtensionCount);//��Ա������reserveָ����С
    std::copy(glfwExtensions, glfwExtensions + glfwExtensionCount, std::back_inserter(glfwExtensions_vec));
    //������char**תvector
    for (const auto glfwExtension : glfwExtensions_vec)
    {
        std::cout << glfwExtension << std::endl;
    }
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

