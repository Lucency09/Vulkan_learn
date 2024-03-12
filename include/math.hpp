#pragma once

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <initializer_list>

namespace toy2d {
    //����ṹ��
    struct Vertex {
        glm::vec3 position;//��������
        glm::vec3 color;//������ɫ
        glm::vec3 normal;//���㷨����

        static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescription();
        static vk::VertexInputBindingDescription GetBindingDescription();
    };
}