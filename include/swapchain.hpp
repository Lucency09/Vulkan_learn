#pragma once
#include "vulkan/vulkan.hpp"

#include <iostream>
#include <optional>

#ifndef SWAPCHAIN
#define SWAPCHAIN

namespace toy2d
{
	struct SwapchainInfo {
		vk::Extent2D imageExtent;//图像大小
		uint32_t imageCount;//图像数量
		vk::SurfaceFormatKHR format;//图像的颜色属性
		vk::SurfaceTransformFlagBitsKHR transform;//几何变换方式
		vk::PresentModeKHR present;//用于(显卡->屏幕)
	};

	class Swapchain final
	{
	public:
		Swapchain(int w, int h);//传参为画布大小
		~Swapchain();

		SwapchainInfo& get_info();
		void creatFramebuffers(int w, int h);

		vk::SwapchainKHR& get_swapchain();
		std::vector<vk::Framebuffer>& get_framebuffers();

	private:
		SwapchainInfo info;
		vk::SwapchainKHR swapchain;//用于存储交换链
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageviews;
		std::vector<vk::Framebuffer> framebuffers;

		void queryInfo(int w, int h);
		void getImages();
		void createImageViews();
		
	};
}
#endif // !SWAPCHAIN