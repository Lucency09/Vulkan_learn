#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

    struct Uniform final {

        static std::vector<vk::DescriptorSetLayoutBinding> GetBinding() 
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(2)//绑定着色器中binding=0
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//是一个UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                //在顶点和片段着色器中都可使用
                .setDescriptorCount(1);//binding=0中有1个元素(设为其他值时可以ubo[2])
            std::vector<vk::DescriptorSetLayoutBinding> bindings = { binding };
            return bindings;
        }
    };

}
