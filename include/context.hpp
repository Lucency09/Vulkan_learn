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
		static Context& GetInstance();//��������ʵ��
		vk::Instance get_instance();//����Vulkan���
		vk::SurfaceKHR get_surface();
		vk::PhysicalDevice& get_phyDevice();
		~Context();
		
	private:
		struct QueueFamliyIndices final
		{
			std::optional<uint32_t> graphicsQueue;//ͼ���������
			std::optional<uint32_t> presentQueue;//�������
			operator bool() const
			{
				return graphicsQueue.has_value() && presentQueue.has_value();
			}
		};

		Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static Context* instance_;//�洢����
		vk::Instance instance = nullptr;//�洢Vulkan���
		vk::PhysicalDevice phyDevice;//�洢�����豸
		vk::Device device;//�洢�߼��豸
		vk::Queue graphcisQueue;//ͼ�������б�
		vk::Queue presentQueue;//�����б�
		vk::SurfaceKHR surface;//���ڴ洢vulkan����
		QueueFamliyIndices queueFamilyIndices;//�洢�������

		void createInstance(std::vector<const char*>& glfwExtensions);//����Vulkan���
		void pickupPhyiscalDevice();//ѡ�������豸
		void creatDevice();//�����߼��豸
		void getQueues();//����graphcisQueueֵ

		void queryQueueFamilyIndices();//��ѯ֧�ֵ��������
		
	};
}