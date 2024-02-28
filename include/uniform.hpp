#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

    struct Uniform final {
        Color color; 

        static vk::DescriptorSetLayoutBinding GetBinding() {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(0)//绑定着色器中binding=0
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//是一个UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment)
                //仅用在Fragment着色器上 ,可加上|vk::ShaderStageFlagBits::eVertex意为顶点片段着色器上都能用
                .setDescriptorCount(1);//binding=0中有1个元素(设为其他值时可以ubo[2])
            return binding;
        }
    };

}
