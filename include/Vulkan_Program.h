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
    VkInstance instance;//�洢Vulkanʵ����ͨ��context��ȡ
    GLFWwindow* window;


    void initWindow();//glfw�������ڵ�һЩ����
    void initVulkan();//Vulkan��ʼ��������context�ĵ���
    void mainLoop();//ѭ���������
    void cleanup();//��Դ�ͷţ�����Vulkan��glfw����
};

void check_Extension();//���֧�ֵ���չ
bool checkValidationLayerSupport();