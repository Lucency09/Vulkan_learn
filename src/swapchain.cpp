#include "swapchain.hpp"

namespace toy2d
{
	Swapchain::Swapchain()
	{
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//�������ߴ���ڴ��ڳߴ�ʱ�Ƿ�ü�
					.setImageArrayLayers(1)//���ý�������ͼ����(��Ļ������)
					
	}
	Swapchain::~Swapchain()
	{
	}
}