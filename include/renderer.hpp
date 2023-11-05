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
		vk::Fence cmdAvaliableFence_;//用于设置CPU和GPU之间的同步

		void initCmdPool();
		void allocCmdBuf();
		void createSems();
		void createFence();//创建Fence对象,其初始状态为非信号态
	};
}