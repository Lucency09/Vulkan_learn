#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d
{
	struct Vertex final//用于存储顶点数据
	{
        //具体数据
		float x, y;

        //数据的布局
        static vk::VertexInputAttributeDescription GetAttribute() {
            vk::VertexInputAttributeDescription attr;
            attr.setBinding(0)
                .setFormat(vk::Format::eR32G32Sfloat)//每个片元2个元素(x,y)
                .setLocation(0)//设置layout(location = 0)
                .setOffset(0);
            return attr;
        }

        static vk::VertexInputBindingDescription GetBinding() {
            vk::VertexInputBindingDescription binding;

            binding.setBinding(0)
                .setInputRate(vk::VertexInputRate::eVertex)//每次以片元为单位传输数据
                .setStride(sizeof(Vertex));//传输长度设置为Vertex的长度

            return binding;
        }
	};
}