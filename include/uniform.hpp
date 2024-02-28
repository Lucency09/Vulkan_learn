#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

    struct Uniform final {
        Color color; 

        static vk::DescriptorSetLayoutBinding GetBinding() {
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(0)//����ɫ����binding=0
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)//��һ��UniformBuffer
                .setStageFlags(vk::ShaderStageFlagBits::eFragment)
                //������Fragment��ɫ���� ,�ɼ���|vk::ShaderStageFlagBits::eVertex��Ϊ����Ƭ����ɫ���϶�����
                .setDescriptorCount(1);//binding=0����1��Ԫ��(��Ϊ����ֵʱ����ubo[2])
            return binding;
        }
    };

}
