#pragma once

#include <vulkan/vulkan.hpp>

namespace toy2d
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void DrawTriangle();

	private:
		int maxFlightCount_ = 2;
		int curFrame_ = 0;

		vk::CommandPool cmdPool_;
		std::vector<vk::CommandBuffer> cmdBuf_;
		std::vector<vk::Semaphore> imageAvaliable_;
		std::vector<vk::Semaphore> imageDrawFinish_;
		std::vector<vk::Fence> cmdAvaliableFence_;//用于设置CPU和GPU之间的同步

		void initCmdPool();
		void allocCmdBuf();
		void createSems();
		void createFence();//创建Fence对象,其初始状态为非信号态
	};
}