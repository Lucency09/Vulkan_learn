#include "context.hpp"


namespace toy2d
{
	Context* Context::instance_ = nullptr;
	void Context::Init()
	{
		instance_ = new Context;
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

	Context::Context()
	{
		createInstance();
		pickupPhyiscalDevice();
		queryQueueFamilyIndices();
		creatDevice();
		getQueues();
	}

	void Context::createInstance()
	{
		vk::InstanceCreateInfo createInfo;
		/*��ѯ����֧�ֵĲ㣬��ʱ����
		auto layers = vk::enumerateInstanceLayerProperties();
		for (auto& layer : layers)
		{
			std::cout << layer.layerName << std::endl;
		}
		*/
		vk::ApplicationInfo appInfo;
		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo);

		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };//�����õ���֤��
		createInfo.setPEnabledLayerNames(layers);//����֤����ؽ�ȥ

		instance = vk::createInstance(createInfo);//����Vulkan���
	}

	void Context::pickupPhyiscalDevice()
	{ 
		auto devices = instance.enumeratePhysicalDevices();//��ȡ�豸�б�
		//���������豸
		for (auto& device : devices)
		{
			auto feature = device.getFeatures();//�豸֧�ֵ�����
			std::cout << device.getProperties().deviceName;//��ӡ�豸����
			std::cout << "  " << device.getProperties().deviceID;//�豸ID
			std::cout << "  " << device.getProperties().apiVersion << std::endl;//api�汾
		}
		//
		phyDevice = devices[0];//ֻ��һ���Կ���������Ϊ��һ��
		
	}

	void Context::creatDevice()
	{
		vk::DeviceCreateInfo devicecreateInfo;
		//createInfo.setPEnabledLayerNames();//�̳�creatInstance�����е���Ϣ�����������߼��豸���е���֤
		vk::DeviceQueueCreateInfo queueCreateInfo;//�������У�
		float priorities = 1.0;//���ȼ������1���0
		queueCreateInfo.setPQueuePriorities(&priorities)
			.setQueueCount(1)//����
			.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());//����

		devicecreateInfo.setQueueCreateInfos(queueCreateInfo);
		device = phyDevice.createDevice(devicecreateInfo);
	}

	void Context::queryQueueFamilyIndices()
	{
		auto properties = phyDevice.getQueueFamilyProperties();//���߼��豸��ѯ
		for (int i = 0; i < properties.size(); i++)
		{
			const auto& property = properties[i];
			if (property.queueFlags | vk::QueueFlagBits::eGraphics)
			{
				queueFamilyIndices.graphicsQueue = i;
				break;
			}
		}

	}

	void Context::getQueues()
	{
		graphcisQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
	}

	Context::~Context()
	{
		device.destroy();
		instance.destroy();
	}


}