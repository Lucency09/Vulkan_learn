#pragma once

#include "vulkan/vulkan.hpp"

#ifndef UNIFORM
#define UNIFORM

namespace toy2d {

    struct Uniform final {

        static std::vector<vk::DescriptorSetLayoutBinding> GetBinding() 
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            bindings.resize(2);//3个uniform

            //COLORUniform
            bindings[1].setBinding(static_cast<int>(UBN::COLOR))//绑定着色器中binding
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//是一个UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                //在顶点和片段着色器中都可使用
                .setDescriptorCount(1);//binding=0中有1个元素(设为其他值时可以ubo[2])

            //MVPUniform
            bindings[0].setBinding(static_cast<int>(UBN::MVP))
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                .setDescriptorCount(3);

            return bindings;
        }
    };
}
#endif // !UNIFORM