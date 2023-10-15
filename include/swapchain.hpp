#pragma once
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>



namespace toy2d
{
	class Swapchain final
	{
	public:
		Swapchain(int w, int h);//传参为画布大小
		~Swapchain();

		void queryInfo(int w, int h);
	private:
		struct SwapchainInfo {
			vk::Extent2D imageExtent;//图像大小
			uint32_t imageCount;//图像数量
			vk::SurfaceFormatKHR format;//图像的颜色属性
			vk::SurfaceTransformFlagBitsKHR transform;//几何变换方式
			vk::PresentModeKHR present;//用于(显卡->屏幕)
		};
		SwapchainInfo info;
		vk::SwapchainKHR swapchain;//用于存储交换链
		
	};
}