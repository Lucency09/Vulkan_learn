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

		void InitRender_process(const std::string& vespath, const std::string& frapath, int h, int w);//������Ⱦ����
		void InitRenderer();
		static Context& GetInstance();//��������ʵ��

		vk::Instance get_instance();//����Vulkan���
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
		static Context* instance_;//�洢����
		vk::Instance instance = nullptr;//�洢Vulkan���
		vk::PhysicalDevice phyDevice;//�洢�����豸
		vk::Device device;//�洢�߼��豸
		vk::Queue graphcisQueue;//ͼ�������б�
		vk::Queue presentQueue;//�����б�
		vk::SurfaceKHR surface;//���ڴ洢vulkan����
		std::unique_ptr<Swapchain> swapchain;//���ڴ洢������
		QueueFamliyIndices queueFamilyIndices;//�洢�������
		std::unique_ptr <RenderProcess> render_process;//�洢��Ⱦ����
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<CommandManager> commandManager;//���ڴ�������з��仺����

		void createInstance(std::vector<const char*>& glfwExtensions);//����Vulkan���
		void pickupPhyiscalDevice();//ѡ�������豸
		void creatDevice();//�����߼��豸
		void getQueues();//����graphcisQueueֵ
		void queryQueueFamilyIndices();//��ѯ֧�ֵ��������
		

		//���ڻ��յ���Context����
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
		//����һ����̬��Ƕ�������
		static Garbo garbo_;

	};
}