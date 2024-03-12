#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>

namespace toy2d {
    //顶点结构体
    struct Vertex {
        glm::vec3 position;//顶点坐标
        glm::vec3 color;//顶点颜色
        glm::vec3 normal;//顶点法向量

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };
}