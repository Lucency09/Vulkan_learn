#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>
#include <random>

/*
* 该文件用于定义一些基本的数据结构及一些图形学计算的函数
*/

#ifndef MATH
#define MATH

namespace toy2d {

    //常量
    constexpr int WINDOWS_WIDTH = 1920;
    constexpr int WINDOWS_HIGHT = 1080;
    
    //Uniform变量binding表
    enum class UBN : uint32_t //Uniform Binding Name
    {
        //TRANSFORM = 0 ,
        MVP = 0 ,
        COLOR = 1,
        SAMPLER = 2
	};
    
    //Uniform内部成员数量
    enum class USC : uint32_t //Uniform Set Count
    {
		MVP = 3,
		COLOR = 1
	};

    //顶点结构体
    struct Vertex 
    {
        glm::vec3 position;//顶点坐标
        glm::vec3 color;//顶点颜色
        glm::vec3 normal;//顶点法向量
        glm::vec2 texCoord;//顶点纹理坐标

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };

    //MVP矩阵
    struct MVP
	{
		glm::mat4 model;//模型矩阵-位置变换
		glm::mat4 view; //观察矩阵-视角变换
		glm::mat4 proj; //投影矩阵-透视变换
	};

    glm::mat4 getrotate(glm::mat4& oldmat4);//获取旋转矩阵
}
#endif // !MATH