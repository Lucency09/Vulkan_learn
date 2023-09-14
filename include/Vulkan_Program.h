#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <iostream>

#include "context.hpp"

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
    void run();
    void Init();
    void Quit();

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    VkInstance instance;//存储Vulkan实例，通过context获取
    GLFWwindow* window;


    void initWindow();//glfw创建窗口的一些操作
    void initVulkan();//Vulkan初始化，单例context的调用
    void mainLoop();//循环输出窗口
    void cleanup();//资源释放，包括Vulkan和glfw窗口
};

void check_Extension();//检查支持的扩展
bool checkValidationLayerSupport();