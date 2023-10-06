#include "context.hpp"


namespace toy2d
{
	Context* Context::instance_ = nullptr;
	void Context::Init(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func)
	{
		instance_ = new Context(glfwExtensions, func);
	}

	void Context::Quit()
	{
		delete instance_;
		instance_ = nullptr;
		
	}

	Context& Context::GetInstance()
	{
		return *instance_;
	}

	vk::Instance Context::get_instance()
	{
		return instance;
	}

	vk::SurfaceKHR Context::get_surface()
	{
		return surface;
	}

	vk::PhysicalDevice& Context::get_phyDevice()
	{
		return phyDevice;
	}

	Context::Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func)
	{
		createInstance(glfwExtensions);
		pickupPhyiscalDevice();
		surface = func(instance);
		queryQueueFamilyIndices();
		creatDevice();
		getQueues();
	}

	void Context::createInstance(std::vector<const char*>& glfwExtensions)
	{
		vk::InstanceCreateInfo createInfo;
		/*查询所有支持的层，暂时不用
		auto layers = vk::enumerateInstanceLayerProperties();
		for (auto& layer : layers)
		{
			std::cout << layer.layerName << std::endl;
		}
		*/
		vk::ApplicationInfo appInfo;
		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo);

		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };//调试用的验证层
		RemoveNosupportedElems<const char*, vk::LayerProperties>(layers, vk::enumerateInstanceLayerProperties(), //查询所有支持的层，暂时不用
			[](const char* e1, const vk::LayerProperties& e2)
				{return std::strcmp(e1, e2.layerName) == 0;});//清除用不上的层
		createInfo.setPEnabledLayerNames(layers)//把验证层加载进去
					.setPEnabledExtensionNames(glfwExtensions);

		instance = vk::createInstance(createInfo);//创建Vulkan句柄
	}

	void Context::pickupPhyiscalDevice()
	{ 
		auto devices = instance.enumeratePhysicalDevices();//获取设备列表
		//遍历所有设备
		for (auto& device : devices)
		{   
			auto feature = device.getFeatures();//设备支持的特性
			//std::cout << device.getProperties().deviceName;//打印设备名称
			//std::cout << "  " << device.getProperties().deviceID;//设备ID
			//std::cout << "  " << device.getProperties().apiVersion << std::endl;//api版本
		}
		//
		phyDevice = devices[0];//只有一张显卡，故设置为第一张
		
	}

	void Context::creatDevice()
	{
		vk::DeviceCreateInfo devicecreateInfo;
		//createInfo.setPEnabledLayerNames();//继承creatInstance步骤中的信息，但可设置逻辑设备独有的验证
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;//命令缓冲队列？
		float priorities = 1.0;//优先级，最高1最低0
		if (queueFamilyIndices.presentQueue.value() == queueFamilyIndices.graphicsQueue.value())
		{//判断图形命令队列和命令队列是否一个队列，从而觉得为GPU创建几个命令队列
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//数量
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());//类型
			queueCreateInfos.push_back(std::move(queueCreateInfo));
		}
		else
		{
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//数量
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());//类型
			queueCreateInfos.push_back(std::move(queueCreateInfo));
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//数量
				.setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());//类型
			queueCreateInfos.push_back(queueCreateInfo);
		}

		devicecreateInfo.setQueueCreateInfos(queueCreateInfos);
		device = phyDevice.createDevice(devicecreateInfo);
	}

	void Context::queryQueueFamilyIndices()
	{
		auto properties = phyDevice.getQueueFamilyProperties();//向逻辑设备查询
		for (int i = 0; i < properties.size(); i++)
		{
			const auto& property = properties[i];
			if (property.queueFlags | vk::QueueFlagBits::eGraphics)
			{//判断是否支持图像队列,当前看作固定写法
				queueFamilyIndices.graphicsQueue = i;
			}
			if (phyDevice.getSurfaceSupportKHR(i, surface))
			{
				queueFamilyIndices.presentQueue = i;
			}
			if (queueFamilyIndices)
			{
				break;
			}
		}

	}

	void Context::getQueues()
	{
		graphcisQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
	}

	Context::~Context()
	{
		instance.destroySurfaceKHR(surface);
		device.destroy();
		instance.destroy();
	}


}