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
		vk::PhysicalDevice& phyDevice =  Context::GetInstance().get_phyDevice();
		auto formats = phyDevice.getSurfaceFormatsKHR();
		info.format = formats[0];
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eR8G8B8A8Srgb &&//8λsrgb
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)//srgb������ɫ�ʿռ�
			{
				info.format = format;
				break;
			}
		}
	}
}