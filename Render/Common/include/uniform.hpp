#pragma once

#include "vulkan/vulkan.hpp"

#ifndef UNIFORM
#define UNIFORM

namespace toy2d {

    struct Uniform final {

        static std::vector<vk::DescriptorSetLayoutBinding> GetBinding() 
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            bindings.resize(2);//3��uniform

            //COLORUniform
            bindings[static_cast<int>(UBN::COLOR)]
                .setBinding(static_cast<int>(UBN::COLOR))//����ɫ����binding
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//��һ��UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                //�ڶ����Ƭ����ɫ���ж���ʹ��
                .setDescriptorCount(1);//binding=0����1��Ԫ��(��Ϊ����ֵʱ����ubo[2])

            //MVPUniform
            bindings[static_cast<int>(UBN::MVP)]
                .setBinding(static_cast<int>(UBN::MVP))
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                .setDescriptorCount(1);

            return bindings;
        }
    };
}
#endif // !UNIFORM