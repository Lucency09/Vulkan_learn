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
		static Context& GetInstance();
		vk::Instance get_instance();//����Vulkan���
		~Context();
		
	private:
		struct QueueFamliyIndices final
		{
			std::optional<uint32_t> graphicsQueue;
		};

		Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static Context* instance_;//�洢����
		vk::Instance instance = nullptr;//�洢Vulkan���
		vk::PhysicalDevice phyDevice;//�洢�����豸
		vk::Device device;//�洢�߼��豸
		vk::Queue graphcisQueue;//ͼ�������б�
		vk::SurfaceKHR surface;//���ڴ洢vulkan����
		QueueFamliyIndices queueFamilyIndices;//�洢�������

		void createInstance(std::vector<const char*>& glfwExtensions);//����Vulkan���
		void pickupPhyiscalDevice();//ѡ�������豸
		void creatDevice();//�����߼��豸
		void getQueues();//����graphcisQueueֵ

		void queryQueueFamilyIndices();//��ѯ֧�ֵ��������
		
	};
}