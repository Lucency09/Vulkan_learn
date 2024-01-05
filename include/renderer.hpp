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

		vk::CommandPool cmdPool_;//������
		std::vector<vk::CommandBuffer> cmdBuf_;//���ڴ洢�����
		std::vector<vk::Semaphore> imageAvaliable_;//���ƻ��������ύ���ź�
		std::vector<vk::Semaphore> imageDrawFinish_;//����ͼ����ʾ���ź�
		std::vector<vk::Fence> cmdAvaliableFence_;//��������CPU��GPU֮��ͬ����դ��

		std::unique_ptr<Buffer> hostVertexBuffer_;//���ض������껺��
		std::unique_ptr<Buffer> deviceVertexBuffer_;
		std::vector<std::unique_ptr<Buffer>> hostUniformBuffer_;//CPU��buffer
		std::vector<std::unique_ptr<Buffer>> deviceUniformBuffer_;//GPU��buffer

		vk::DescriptorPool descriptorPool_;
		std::vector<vk::DescriptorSet> sets_;

		void initCmdPool();//�����ã�����ת�Ƶ�createCmdBuffers()
		void allocCmdBuf();//�����ã�����ת�Ƶ�createCmdBuffers()
		void createSems();
		void createFence();//����Fence����,���ʼ״̬Ϊ���ź�̬
		void createSemaphores();//����Semaphore����
		void createCmdBuffers();
		void createVertexBuffer();
		void createUniformBuffers();
		void bufferVertexData();//���������ݴ�CPU���䵽GPU
		void bufferUniformData();
		void createDescriptorPool();
		void allocateSets();
		void updateSets();

		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
	};
}