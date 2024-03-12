#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

    struct Uniform final {

        static std::vector<vk::DescriptorSetLayoutBinding> GetBinding() 
        {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(2)//����ɫ����binding=0
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//��һ��UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
                //�ڶ����Ƭ����ɫ���ж���ʹ��
                .setDescriptorCount(1);//binding=0����1��Ԫ��(��Ϊ����ֵʱ����ubo[2])
            std::vector<vk::DescriptorSetLayoutBinding> bindings = { binding };
            return bindings;
        }
    };

}
