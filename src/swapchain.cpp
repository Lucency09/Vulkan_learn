#include "swapchain.hpp"

namespace toy2d
{
	Swapchain::Swapchain()
	{
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//当画布尺寸大于窗口尺寸时是否裁剪
					.setImageArrayLayers(1)//设置交换链视图数量(屏幕数量？)
					
	}
	Swapchain::~Swapchain()
	{
	}
}