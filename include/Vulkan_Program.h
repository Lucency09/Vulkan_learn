#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <cstdlib>

class Vulkan_program
{
public:
    void run();

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    VkInstance instance;//�洢Vulkanʵ��
    GLFWwindow* window;

    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void createInstance();//����Vulkanʵ��
};