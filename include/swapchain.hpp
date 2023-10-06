#pragma once

#include "vulkan/vulkan.hpp"

#include "context.hpp"

namespace toy2d
{
	class Swapchain final
	{
	public:
		Swapchain();
		~Swapchain();
	private:
		struct SwapchainInfo {
			vk::Extent2D imageExtent;//图像大小
			uint32_t imageCount;//图像数量
			vk::SurfaceFormatKHR format;//图像的颜色属性
		};
		SwapchainInfo info;
		vk::SwapchainKHR swapchain;
		void queryInfo();
	};
}