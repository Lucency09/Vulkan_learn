#include "swapchain.hpp"

namespace toy2d
{
	Swapchain::Swapchain()
	{
		queryInfo();
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//当画布尺寸大于窗口尺寸时是否裁剪
			.setImageArrayLayers(1)//设置交换链视图数量(屏幕数量？)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//颜色附件？运行GUP绘制彩色像素点？
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//设置色彩不透明(不与窗口外？其他色彩混合)
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