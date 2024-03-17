#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>

/*
* ���ļ����ڶ���һЩ���������ݽṹ��һЩͼ��ѧ����ĺ���
*/

#ifndef MATH
#define MATH

namespace toy2d {
    
    //Uniform����binding��
    enum class UBN : uint32_t //Uniform Binding Name
    {
        //TRANSFORM = 0 ,
        MVP = 0 ,
        COLOR = 1
	};

    //����ṹ��
    struct Vertex 
    {
        glm::vec3 position;//��������
        glm::vec3 color;//������ɫ
        glm::vec3 normal;//���㷨����

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };
}
#endif // !MATH