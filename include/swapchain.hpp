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
			vk::Extent2D imageExtent;//ͼ���С
			uint32_t imageCount;//ͼ������
			vk::SurfaceFormatKHR format;//ͼ�����ɫ����
		};
		SwapchainInfo info;
		vk::SwapchainKHR swapchain;
		void queryInfo();
	};
}