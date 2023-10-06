#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>

#include "tool.hpp"

namespace toy2d
{
	class Context final
	{
	public:
		static void Init(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static void Quit();
		static Context& GetInstance();//返回自身实例
		vk::Instance get_instance();//返回Vulkan句柄
		vk::SurfaceKHR get_surface();
		vk::PhysicalDevice& get_phyDevice();
		~Context();
		
	private:
		struct QueueFamliyIndices final
		{
			std::optional<uint32_t> graphicsQueue;//图像命令队列
			std::optional<uint32_t> presentQueue;//命令队列
			operator bool() const
			{
				return graphicsQueue.has_value() && presentQueue.has_value();
			}
		};

		Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static Context* instance_;//存储自身
		vk::Instance instance = nullptr;//存储Vulkan句柄
		vk::PhysicalDevice phyDevice;//存储物理设备
		vk::Device device;//存储逻辑设备
		vk::Queue graphcisQueue;//图像命令列表
		vk::Queue presentQueue;//命令列表
		vk::SurfaceKHR surface;//用于存储vulkan画布
		QueueFamliyIndices queueFamilyIndices;//存储命令队列

		void createInstance(std::vector<const char*>& glfwExtensions);//创建Vulkan句柄
		void pickupPhyiscalDevice();//选择物理设备
		void creatDevice();//创建逻辑设备
		void getQueues();//设置graphcisQueue值

		void queryQueueFamilyIndices();//查询支持的命令队列
		
	};
}