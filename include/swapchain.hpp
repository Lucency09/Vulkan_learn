#pragma once
#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>



namespace toy2d
{
	class Swapchain final
	{
	public:
		Swapchain(int w, int h);//����Ϊ������С
		~Swapchain();

		void queryInfo(int w, int h);
	private:
		struct SwapchainInfo {
			vk::Extent2D imageExtent;//ͼ���С
			uint32_t imageCount;//ͼ������
			vk::SurfaceFormatKHR format;//ͼ�����ɫ����
			vk::SurfaceTransformFlagBitsKHR transform;//���α任��ʽ
			vk::PresentModeKHR present;//����(�Կ�->��Ļ)
		};
		SwapchainInfo info;
		vk::SwapchainKHR swapchain;//���ڴ洢������
		
	};
}