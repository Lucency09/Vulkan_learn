#include "swapchain.hpp"

namespace toy2d
{
	Swapchain::Swapchain()
	{
		queryInfo();
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//�������ߴ���ڴ��ڳߴ�ʱ�Ƿ�ü�
			.setImageArrayLayers(1)//���ý�������ͼ����(��Ļ������)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//��ɫ����������GUP���Ʋ�ɫ���ص㣿
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//����ɫ�ʲ�͸��(���봰���⣿����ɫ�ʻ��)
			.setSurface(Context::GetInstance().get_surface());
	}
	Swapchain::~Swapchain()
	{
	}
	void Swapchain::queryInfo()
	{
		Context::GetInstance().
	}
}