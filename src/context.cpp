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
		createInfo.setPEnabledLayerNames(layers);//把验证层加载进去

		instance = vk::createInstance(createInfo);//创建Vulkan句柄
	}
	Context::~Context()
	{
		instance.destroy();
	}


}