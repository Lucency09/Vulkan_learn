#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>

/*
* 该文件用于定义一些基本的数据结构及一些图形学计算的函数
*/

#ifndef MATH
#define MATH

namespace toy2d {
    
    //Uniform变量binding表
    enum class UBN : uint32_t //Uniform Binding Name
    {
        //TRANSFORM = 0 ,
        MVP = 0 ,
        COLOR = 1
	};

    //顶点结构体
    struct Vertex 
    {
        glm::vec3 position;//顶点坐标
        glm::vec3 color;//顶点颜色
        glm::vec3 normal;//顶点法向量

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };
}
#endif // !MATH