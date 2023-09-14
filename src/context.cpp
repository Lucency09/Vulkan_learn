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
	Context::~Context()
	{
		instance.destroy();
	}


}