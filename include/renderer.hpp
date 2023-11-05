#pragma once

#include <vulkan/vulkan.hpp>

namespace toy2d
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void Render();

	private:
		vk::CommandPool cmdPool_;
		vk::CommandBuffer cmdBuf_;
		vk::Semaphore imageAvaliable_;
		vk::Semaphore imageDrawFinish_;
		vk::Fence cmdAvaliableFence_;//��������CPU��GPU֮���ͬ��

		void initCmdPool();
		void allocCmdBuf();
		void createSems();
		void createFence();//����Fence����,���ʼ״̬Ϊ���ź�̬
	};
}