#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>
#include <random>

/*
* ���ļ����ڶ���һЩ���������ݽṹ��һЩͼ��ѧ����ĺ���
*/

#ifndef MATH
#define MATH

namespace toy2d {

    //����
    constexpr int WINDOWS_WIDTH = 1920;
    constexpr int WINDOWS_HIGHT = 1080;
    
    //Uniform����binding��
    enum class UBN : uint32_t //Uniform Binding Name
    {
        //TRANSFORM = 0 ,
        MVP = 0 ,
        COLOR = 1,
        SAMPLER = 2
	};
    
    //Uniform�ڲ���Ա����
    enum class USC : uint32_t //Uniform Set Count
    {
		MVP = 3,
		COLOR = 1
	};

    //����ṹ��
    struct Vertex 
    {
        glm::vec3 position;//��������
        glm::vec3 color;//������ɫ
        glm::vec3 normal;//���㷨����
        glm::vec2 texCoord;//������������

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };

    //MVP����
    struct MVP
	{
		glm::mat4 model;//ģ�;���-λ�ñ任
		glm::mat4 view; //�۲����-�ӽǱ任
		glm::mat4 proj; //ͶӰ����-͸�ӱ任
	};

    glm::mat4 getrotate(glm::mat4& oldmat4);//��ȡ��ת����
}
#endif // !MATH