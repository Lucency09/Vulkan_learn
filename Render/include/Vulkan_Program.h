#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <iostream>
#include <Windows.h>

#include "Render/Vulkan/include/context.hpp"
#include "Control/include/camera.hpp"


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class Vulkan_program
{
public:
    Vulkan_program();
    ~Vulkan_program();
    void run();
    void Init();
    void Quit();

private:
    VkInstance instance;//存储Vulkan实例，通过context获取
    GLFWwindow* window;
    std::vector<const char*> glfwExtensions_vec;//存储需要的扩展
    


    void initWindow();//glfw创建窗口的一些操作
    void initVulkan();//Vulkan初始化，单例context的调用
    void mainLoop();//循环输出窗口
    void cleanup();//资源释放，包括Vulkan和glfw窗口
    void check_glfwExtension();//检查glfw所需要的的扩展
};

void check_Extension();//检查支持的扩展
bool checkValidationLayerSupport();
