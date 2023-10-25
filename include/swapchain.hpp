#pragma once
#include "vulkan/vulkan.hpp"

#include <iostream>
#include <optional>



namespace toy2d
{
	struct SwapchainInfo {
		vk::Extent2D imageExtent;//ͼ���С
		uint32_t imageCount;//ͼ������
		vk::SurfaceFormatKHR format;//ͼ�����ɫ����
		vk::SurfaceTransformFlagBitsKHR transform;//���α任��ʽ
		vk::PresentModeKHR present;//����(�Կ�->��Ļ)
	};

	class Swapchain final
	{
	public:
		Swapchain(int w, int h);//����Ϊ������С
		~Swapchain();

		SwapchainInfo& get_info();

	private:
		

		SwapchainInfo info;
		vk::SwapchainKHR swapchain;//���ڴ洢������
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageviews;

		void queryInfo(int w, int h);
		void getImages();
		void createImageViews();
	};
}