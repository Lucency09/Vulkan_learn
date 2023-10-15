#include "swapchain.hpp"
#include "context.hpp"
namespace toy2d
{
	Swapchain::Swapchain(int w, int h)
	{
		queryInfo(w, h);//����info������Ϊ����һ��set��׼��
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)//�������ߴ���ڴ��ڳߴ�ʱ�Ƿ�ü�
			.setImageArrayLayers(1)//���ý�������ͼ����(��Ļ������)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)//��ɫ����������GUP���Ʋ�ɫ���ص㣿
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)//����ɫ�ʲ�͸��(���봰���⣿����ɫ�ʻ��)
			.setSurface(Context::GetInstance().get_surface())
			.setImageColorSpace(info.format.colorSpace)//ɫ�ʿռ�
			.setImageFormat(info.format.format)
			.setImageExtent(info.imageExtent)
			.setMinImageCount(info.imageCount)
			.setPreTransform(info.transform)
			.setPresentMode(info.present);//����ͼ����ƶ��еĹ���ʽ

		auto& queueIndicecs = Context::GetInstance().get_queueFamilyIndices();//��ȡ�������
		if (queueIndicecs.graphicsQueue.value() == queueIndicecs.presentQueue.value())
		{//��֧��һ���������
			createInfo.setQueueFamilyIndices(queueIndicecs.graphicsQueue.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else
		{//���֧�������������
			std::array indices = { queueIndicecs.graphicsQueue.value(), queueIndicecs.presentQueue.value() };
			createInfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);//����ģʽ
		}

		swapchain = Context::GetInstance().get_device().createSwapchainKHR(createInfo);
	}

	Swapchain::~Swapchain()
	{
		for (const auto& view : imageviews)
		{
			Context::GetInstance().get_device().destroyImageView(view);
		}
		Context::GetInstance().get_device().destroySwapchainKHR(swapchain);
	}

	void Swapchain::queryInfo(int w, int h)
	{
		vk::PhysicalDevice& phyDevice =  Context::GetInstance().get_phyDevice();//��ȡGPU
		vk::SurfaceKHR& surface = Context::GetInstance().get_surface();
		auto formats = phyDevice.getSurfaceFormatsKHR(surface);//�ҵ����ʵ�format��ʽ
		info.format = formats[0];//����Ĭ��ֵ
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eR8G8B8A8Srgb &&//8λsrgb
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)//srgb������ɫ�ʿռ�
			{
				info.format = format;
				break;
			}
		}
		 //ͼ�����
		vk::SurfaceCapabilitiesKHR capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);//2��surface�����ڻ������
		//						std::clamp<>(a, b, c)  if(a<b){return b} else if(a > c){return c}else{return a} 
		//��ȡ�����ߴ�
		info.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		info.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		//���ü��α任��ʽ
		info.transform = capabilities.currentTransform;//Ĭ�ϲ��任
		auto presents = phyDevice.getSurfacePresentModesKHR(surface);//�ҵ����ʵ�present
		info.present = vk::PresentModeKHR::eFifo;//����Ĭ��ֵ���Ƚ��ȳ�
		for (const auto& present : presents)
		{
			if (present == vk::PresentModeKHR::eMailbox)//��ſ������ɶ��г���Ϊ1��FIFO������������� ����һ�λ��Ƶ�ͼ��ʼ�������µ�
			{
				info.present = present;
				break;
			}
		}
	}

	void Swapchain::getImages()
	{
		images = Context::GetInstance().get_device().getSwapchainImagesKHR(swapchain);
	}

	void Swapchain::createImageViews()
	{
		imageviews.resize(images.size());
		for (int i = 0; i < images.size(); i++)
		{
			vk::ImageViewCreateInfo creatinfo;
			vk::ComponentMapping mapping;//���ڸ�����ɫͨ����ӳ�䣿Ĭ��RGBA������
			vk::ImageSubresourceRange range;
			range.setBaseMipLevel(0)//����mipmap����
				.setLayerCount(1)//mipmap����
				.setBaseArrayLayer(0)// �����������������Դ��Χ�Ļ�������㼶Ϊ0��Ҳ���ǵ�һ������Ԫ�ء�����㼶���ڴ洢���ͼ����Դ��������������ͼ��3D����
				.setLayerCount(1)//�����������������Դ��Χ������㼶��Ϊ1��Ҳ����ֻ��һ������Ԫ�ء����Ҫ���ʶ������Ԫ�أ���Ҫ����ʵ������������������
				.setAspectMask(vk::ImageAspectFlagBits::eColor);//�����������������Դ��Χ������Դ����Ϊ��ɫ��Ҳ����ֻ����ͼ����Դ�е���ɫ���ݡ��������ܵ����Ͱ�����ȡ�ģ���Ԫ���ݵȡ�
			creatinfo.setImage(images[i])
				.setViewType(vk::ImageViewType::e2D)//2Dͼ��
				.setComponents(mapping)
				.setFormat(info.format.format)
				.setSubresourceRange(range);
			imageviews[i] = Context::GetInstance().get_device().createImageView(creatinfo);
		}
	}
}