#pragma once

#include "vulkan/vulkan.hpp"

#ifndef UNIFORM
#define UNIFORM

namespace toy2d {

    struct Uniform final {

        static std::vector<vk::DescriptorSetLayoutBinding> GetBinding() 
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            bindings.resize(3);//3个uniform

            //COLORUniform
            bindings[static_cast<int>(UBN::COLOR)]
                .setBinding(static_cast<int>(UBN::COLOR))//绑定着色器中binding
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//是一个UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                //在顶点和片段着色器中都可使用
                .setDescriptorCount(1);//binding=0中有1个元素(设为其他值时可以ubo[2])

            //MVPUniform
            bindings[static_cast<int>(UBN::MVP)]
                .setBinding(static_cast<int>(UBN::MVP))
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                .setDescriptorCount(1);

            //Sampler
            bindings[static_cast<int>(UBN::SAMPLER)]
                .setBinding(static_cast<int>(UBN::SAMPLER))
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setStageFlags(vk::ShaderStageFlagBits::eFragment)
                .setDescriptorCount(1);


            return bindings;
        }
    };
}
#endif // !UNIFORM