#pragma once

#include <vulkan/vulkan.hpp>

#include "math.hpp"
#include "buffer.hpp"
#include "uniform.hpp"


namespace toy2d
{

	class Renderer final
	{
	public:
		Renderer();
		~Renderer();

		void Draw();
		void set_Vertices();
		void set_Indices();

	private:
		int maxFlightCount_ = 2;
		int curFrame_ = 0;
		std::vector<Vec> vertices = {
			Vec{{-0.5, 0.5, 0}},
			Vec{{-0.5, -0.5, 0}},
			Vec{{0.5, 0.5, 0}},
			Vec{{0.5, -0.5, 0}},
		};
		std::vector<std::uint32_t> indices {
			0, 1, 2,
			2, 1, 3
		};

		Uniform uniform{ Color{1, 0.5, 0} };
		

		vk::CommandPool cmdPool_;//������
		std::vector<vk::CommandBuffer> cmdBuf_;//���ڴ洢�����
		std::vector<vk::Semaphore> imageAvaliable_;//���ƻ��������ύ���ź�
		std::vector<vk::Semaphore> imageDrawFinish_;//����ͼ����ʾ���ź�
		std::vector<vk::Fence> cmdAvaliableFence_;//��������CPU��GPU֮��ͬ����դ��

		std::unique_ptr<Buffer> hostVertexBuffer_;//���ض������껺��
		std::unique_ptr<Buffer> deviceVertexBuffer_;
		std::unique_ptr<Buffer> hostIndicesBuffer_;//���ض�����������
		std::unique_ptr<Buffer> deviceIndicesBuffer_;

		std::vector<std::unique_ptr<Buffer>> hostUniformBuffer_;//CPU��buffer
		std::vector<std::unique_ptr<Buffer>> deviceUniformBuffer_;//GPU��buffer

		vk::DescriptorPool descriptorPool_;
		std::vector<vk::DescriptorSet> sets_;

		void initCmdPool();//�����ã�����ת�Ƶ�createCmdBuffers()
		void allocCmdBuf();//�����ã�����ת�Ƶ�createCmdBuffers()

		void createFence();//����Fence����,���ʼ״̬Ϊ���ź�̬
		void createSemaphores();//����Semaphore����
		void createCmdBuffers();
		void createVertexBuffer();
		void createIndicesBuffer();
		
		void createBuffer();//����verticesBuffer��indicesBuffer
		void updateBuffer();//����verticesBuffer��indicesBuffer
		void createUniformBuffers(); //����Uniform���ݴ��仺��
		void updateVertexData();//���������ݴ�CPU���䵽GPU
		void updateIndicesData();//���������ݴ�CPU���䵽GPU
		void updateUniformData();//��Uniform�����д�������
		void createDescriptorPool();
		void allocateSets();
		void updateSets();

		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
	};
}