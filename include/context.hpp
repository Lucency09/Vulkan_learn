#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>

namespace toy2d
{
	class Context final
	{
	public:
		static void Init();
		static void Quit();
		static Context& GetInstance();
		vk::Instance get_instance();//����Vulkan���
		~Context();
		
	private:
		struct QueueFamliyIndices final
		{
			std::optional<uint32_t> graphicsQueue;
		};

		Context();
		static Context* instance_;//�洢����
		vk::Instance instance = nullptr;//�洢Vulkan���
		vk::PhysicalDevice phyDevice;//�洢�����豸
		vk::Device device;//�洢�߼��豸
		vk::Queue graphcisQueue;//ͼ�������б�
		QueueFamliyIndices queueFamilyIndices;//�洢�������

		void createInstance();//����Vulkan���
		void pickupPhyiscalDevice();//ѡ�������豸
		void creatDevice();//�����߼��豸
		void getQueues();//����graphcisQueueֵ

		void queryQueueFamilyIndices();//��ѯ֧�ֵ��������
		
	};
}