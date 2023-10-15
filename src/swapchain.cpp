#include "swapchain.hpp"
#include "context.hpp"
namespace toy2d
{
	Swapchain::Swapchain(int w, int h)
	{
		queryInfo(w, h);//设置info变量，为后续一堆set做准备
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//当画布尺寸大于窗口尺寸时是否裁剪
			.setImageArrayLayers(1)//设置交换链视图数量(屏幕数量？)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//颜色附件？运行GUP绘制彩色像素点？
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//设置色彩不透明(不与窗口外？其他色彩混合)
			.setSurface(Context::GetInstance().get_surface())
			.setImageColorSpace(info.format.colorSpace)//色彩空间
			.setImageFormat(info.format.format)
			.setImageExtent(info.imageExtent)
			.setMinImageCount(info.imageCount)
			.setPreTransform(info.transform)
			.setPresentMode(info.present);//设置图像绘制队列的管理方式

		auto& queueIndicecs = Context::GetInstance().get_queueFamilyIndices();//获取命令队列
		if (queueIndicecs.graphicsQueue.value() == queueIndicecs.presentQueue.value())
		{//仅支持一个命令队列
			createInfo.setQueueFamilyIndices(queueIndicecs.graphicsQueue.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else
		{//如果支持两个命令队列
			std::array indices = { queueIndicecs.graphicsQueue.value(), queueIndicecs.presentQueue.value() };
			createInfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);//并行模式
		}

		swapchain = Context::GetInstance().get_device().createSwapchainKHR(createInfo);
	}
	Swapchain::~Swapchain()
	{
		Context::GetInstance().get_device().destroySwapchainKHR(swapchain);
	}
	void Swapchain::queryInfo(int w, int h)
	{
		vk::PhysicalDevice& phyDevice =  Context::GetInstance().get_phyDevice();//获取GPU
		vk::SurfaceKHR& surface = Context::GetInstance().get_surface();
		auto formats = phyDevice.getSurfaceFormatsKHR(surface);//找到合适的format格式
		info.format = formats[0];//给个默认值
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eR8G8B8A8Srgb &&//8位srgb
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)//srgb非线性色彩空间
			{
				info.format = format;
				break;
			}
		}
		 //图像个数
		vk::SurfaceCapabilitiesKHR capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);//2个surface层用于绘制最好
		//						std::clamp<>(a, b, c)  if(a<b){return b} else if(a > c){return c}else{return a} 
		//获取画布尺寸
		info.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		info.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		//设置几何变换方式
		info.transform = capabilities.currentTransform;//默认不变换
		auto presents = phyDevice.getSurfacePresentModesKHR(surface);//找到合适的present
		info.present = vk::PresentModeKHR::eFifo;//给个默认值，先进先出
		for (const auto& present : presents)
		{
			if (present == vk::PresentModeKHR::eMailbox)//大概可以理解成队列长度为1的FIFO，多余的舍弃掉 ，下一次绘制的图像始终是最新的
			{
				info.present = present;
				break;
			}
		}
	}
}