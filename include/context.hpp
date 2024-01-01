#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>

#include "tool.hpp"
#include "swapchain.hpp"
#include "render_process.hpp"
#include "renderer.hpp"
#include "command_manager.hpp"

namespace toy2d
{
	struct QueueFamliyIndices final
	{
		std::optional<uint32_t> graphicsQueue;//图像命令队列
		std::optional<uint32_t> presentQueue;//命令队列
		operator bool() const
		{
			return graphicsQueue.has_value() && presentQueue.has_value();
		}
	};

	class Context final
	{
	public:
		static void Init(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static void Quit();

		void InitSwapchain(int w, int h); //创建交换链
		void DestroySwapchain(); //销毁交换链

		void InitRender_process(const std::string& vespath, const std::string& frapath, int h, int w);//创建渲染流程
		void InitRenderer();
		static Context& GetInstance();//返回自身实例

		vk::Instance get_instance();//返回Vulkan句柄
		vk::SurfaceKHR& get_surface();
		vk::PhysicalDevice& get_phyDevice();
		vk::Device& get_device();
		vk::Queue& get_graphcisQueue();
		vk::Queue& get_presentQueue();
		QueueFamliyIndices& get_queueFamilyIndices();
		std::unique_ptr<Swapchain>& get_swapchain();
		RenderProcess& get_render_process();
		Renderer& get_renderer();
		void initCommandPool();
		std::unique_ptr<CommandManager>& get_commandManager();
		
		
	private:
		Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		~Context();
		static Context* instance_;//存储自身
		vk::Instance instance = nullptr;//存储Vulkan句柄
		vk::PhysicalDevice phyDevice;//存储物理设备
		vk::Device device;//存储逻辑设备
		vk::Queue graphcisQueue;//图像命令列表
		vk::Queue presentQueue;//命令列表
		vk::SurfaceKHR surface;//用于存储vulkan画布
		std::unique_ptr<Swapchain> swapchain;//用于存储交换链
		QueueFamliyIndices queueFamilyIndices;//存储命令队列
		std::unique_ptr <RenderProcess> render_process;//存储渲染流程
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<CommandManager> commandManager;//用于从命令池中分配缓冲区

		void createInstance(std::vector<const char*>& glfwExtensions);//创建Vulkan句柄
		void pickupPhyiscalDevice();//选择物理设备
		void creatDevice();//创建逻辑设备
		void getQueues();//设置graphcisQueue值
		void queryQueueFamilyIndices();//查询支持的命令队列
		

		//用于回收单例Context对象
		class Garbo
		{
		public:
			~Garbo()
			{
				if (Context::instance_)
				{
					delete Context::instance_;
					Context::instance_ = nullptr;
				}
			}
		};
		//定义一个静态的嵌套类对象
		static Garbo garbo_;

	};
}