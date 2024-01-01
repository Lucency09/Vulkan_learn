#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d
{
	struct Vertex final//���ڴ洢��������
	{
        //��������
		float x, y;

        //���ݵĲ���
        static vk::VertexInputAttributeDescription GetAttribute() {
            vk::VertexInputAttributeDescription attr;
            attr.setBinding(0)
                .setFormat(vk::Format::eR32G32Sfloat)//ÿ��ƬԪ2��Ԫ��(x,y)
                .setLocation(0)//����layout(location = 0)
                .setOffset(0);
            return attr;
        }

        static vk::VertexInputBindingDescription GetBinding() {
            vk::VertexInputBindingDescription binding;

            binding.setBinding(0)
                .setInputRate(vk::VertexInputRate::eVertex)//ÿ����ƬԪΪ��λ��������
                .setStride(sizeof(Vertex));//���䳤������ΪVertex�ĳ���

            return binding;
        }
	};
}