#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>
#include <optional>

#include "tool.hpp"
#include "swapchain.hpp"

namespace toy2d
{
	struct QueueFamliyIndices final
	{
		std::optional<uint32_t> graphicsQueue;//ͼ���������
		std::optional<uint32_t> presentQueue;//�������
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

		void InitSwapchain(int w, int h); //����������
		void DestroySwapchain(); //���ٽ�����

		static Context& GetInstance();//��������ʵ��
		vk::Instance get_instance();//����Vulkan���
		vk::SurfaceKHR& get_surface();
		vk::PhysicalDevice& get_phyDevice();
		vk::Device& get_device();
		QueueFamliyIndices& get_queueFamilyIndices();
		std::unique_ptr<Swapchain>& get_swapchain();
		~Context();
		
	private:

		Context(std::vector<const char*>& glfwExtensions, CreateSurfaceFunc func);
		static Context* instance_;//�洢����
		vk::Instance instance = nullptr;//�洢Vulkan���
		vk::PhysicalDevice phyDevice;//�洢�����豸
		vk::Device device;//�洢�߼��豸
		vk::Queue graphcisQueue;//ͼ�������б�
		vk::Queue presentQueue;//�����б�
		vk::SurfaceKHR surface;//���ڴ洢vulkan����
		std::unique_ptr<Swapchain> swapchain;//���ڴ洢������
		QueueFamliyIndices queueFamilyIndices;//�洢�������

		void createInstance(std::vector<const char*>& glfwExtensions);//����Vulkan���
		void pickupPhyiscalDevice();//ѡ�������豸
		void creatDevice();//�����߼��豸
		void getQueues();//����graphcisQueueֵ

		void queryQueueFamilyIndices();//��ѯ֧�ֵ��������

	};
}