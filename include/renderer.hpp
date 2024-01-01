#pragma once

#include <vulkan/vulkan.hpp>

#include "vertex.hpp"
#include "buffer.hpp"

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

		std::unique_ptr<Buffer> hostVertexBuffer_;
		std::unique_ptr<Buffer> deviceVertexBuffer_;
		std::vector<std::unique_ptr<Buffer>> hostUniformBuffer_;
		std::vector<std::unique_ptr<Buffer>> deviceUniformBuffer_;

		vk::DescriptorPool descriptorPool_;
		std::vector<vk::DescriptorSet> sets_;

		void initCmdPool();
		void allocCmdBuf();
		void createSems();
		void createFence();//����Fence����,���ʼ״̬Ϊ���ź�̬
		void createSemaphores();//����Semaphore����
		void createCmdBuffers();
		void createVertexBuffer();
		void createUniformBuffers();
		void bufferVertexData();
		void bufferUniformData();
		void createDescriptorPool();
		void allocateSets();
		void updateSets();

		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
	};
}