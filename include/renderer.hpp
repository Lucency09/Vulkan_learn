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
		std::vector<vk::Fence> cmdAvaliableFence_;//��������CPU��GPU֮���ͬ��

		void initCmdPool();
		void allocCmdBuf();
		void createSems();
		void createFence();//����Fence����,���ʼ״̬Ϊ���ź�̬
	};
}