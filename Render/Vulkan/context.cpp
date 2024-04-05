#include "Render/Vulkan/include/context.hpp"


namespace toy2d
{
	Context* Context::instance_ = nullptr;
	void Context::Init(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func)
	{
		instance_ = new Context(glfwExtensions, func);
	}

	void Context::Quit()
	{
		instance_->device.waitIdle();
		instance_->renderer.reset();
		instance_->render_process.reset();
		instance_->DestroySwapchain();

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

	vk::SurfaceKHR& Context::get_surface()
	{
		return surface;
	}

	vk::PhysicalDevice& Context::get_phyDevice()
	{
		return phyDevice;
	}

	vk::Device& Context::get_device()
	{
		return device;
	}

	vk::Queue& Context::get_graphcisQueue()
	{
		return graphcisQueue;
	}

	vk::Queue& Context::get_presentQueue()
	{
		return presentQueue;
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

	Context::~Context()
	{
		commandManager.reset();
		instance.destroySurfaceKHR(surface);
		device.destroy();
		instance.destroy();
		Quit();
	}

	void Context::createInstance(std::vector<const char*>& glfwExtensions)
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
		RemoveNosupportedElems<const char*, vk::LayerProperties>(layers, vk::enumerateInstanceLayerProperties(), //��ѯ����֧�ֵĲ㣬��ʱ����
			[](const char* e1, const vk::LayerProperties& e2)
				{return std::strcmp(e1, e2.layerName) == 0;});//����ò��ϵĲ�
		createInfo.setPEnabledLayerNames(layers)//����֤����ؽ�ȥ
					.setPEnabledExtensionNames(glfwExtensions);

		instance = vk::createInstance(createInfo);//����Vulkan���
	}

	void Context::pickupPhyiscalDevice()
	{ 
		auto devices = instance.enumeratePhysicalDevices();//��ȡ�豸�б�
		//���������豸
		for (auto& device : devices)
		{   
			auto feature = device.getFeatures();//�豸֧�ֵ�����
			//std::cout << device.getProperties().deviceName;//��ӡ�豸����
			//std::cout << "  " << device.getProperties().deviceID;//�豸ID
			//std::cout << "  " << device.getProperties().apiVersion << std::endl;//api�汾
		}
		//
		phyDevice = devices[0];//ֻ��һ���Կ���������Ϊ��һ��
		
	}

	void Context::creatDevice()
	{
		vk::DeviceCreateInfo devicecreateInfo;
		//createInfo.setPEnabledLayerNames();//�̳�creatInstance�����е���Ϣ�����������߼��豸���е���֤
		std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };//���ý�������չ
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;//�������У�
		float priorities = 1.0;//���ȼ������1���0
		if (queueFamilyIndices.presentQueue.value() == queueFamilyIndices.graphicsQueue.value())
		{//�ж�ͼ��������к���������Ƿ�һ�����У��Ӷ�����ΪGPU���������������
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//����
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());//����
			queueCreateInfos.push_back(std::move(queueCreateInfo));
		}
		else
		{
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//����
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());//����
			queueCreateInfos.push_back(queueCreateInfo);
			
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)//����
				.setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());//����
			queueCreateInfos.push_back(queueCreateInfo);
		}

		devicecreateInfo.setQueueCreateInfos(queueCreateInfos)//�����������
					.setPEnabledExtensionNames(extensions);//���ý�����
		device = phyDevice.createDevice(devicecreateInfo);
	}

	void Context::queryQueueFamilyIndices()
	{
		auto properties = phyDevice.getQueueFamilyProperties();//���߼��豸��ѯ
		for (int i = 0; i < properties.size(); i++)
		{
			const auto& property = properties[i];
			if (property.queueFlags | vk::QueueFlagBits::eGraphics)
			{//�ж��Ƿ�֧��ͼ�����,��ǰ�����̶�д��
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

	QueueFamliyIndices& Context::get_queueFamilyIndices()
	{
		return queueFamilyIndices;
	}

	std::unique_ptr<Swapchain>& Context::get_swapchain()
	{
		return swapchain;
	}

	toy2d::RenderProcess& Context::get_render_process()
	{
		return *render_process;
	}

	Renderer& Context::get_renderer()
	{
		return *renderer;
	}

	void Context::initCommandPool()
	{
		this->commandManager = std::make_unique<CommandManager>();
	}

	std::unique_ptr<CommandManager>& Context::get_commandManager()
	{
		return this->commandManager;
	}

	void Context::InitSwapchain(int w, int h)
	{
		
		swapchain.reset(new Swapchain(w, h));
	}

	void Context::DestroySwapchain()
	{
		swapchain.reset();
	}

	void Context::InitRender_process(const std::string& vespath, const std::string& frapath, int h, int w)
	{
		render_process.reset(new RenderProcess);
		std::cout << vespath << std::endl << frapath << std::endl;
		instance_->render_process->InitPipeLine(toy2d::Read_spv_File(vespath),
			toy2d::Read_spv_File(frapath), w, h);
		instance_->get_swapchain()->creatFramebuffers(w, h);
	}

	void Context::InitRenderer()
	{
		renderer.reset(new Renderer);
	}

	void Context::getQueues()
	{
		graphcisQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
	}
}