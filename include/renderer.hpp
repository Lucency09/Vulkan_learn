#pragma once

#include <vulkan/vulkan.hpp>
#include <map>

#include "math.hpp"
#include "buffer.hpp"
#include "uniform.hpp"

#ifndef RENDERER
#define RENDERER

namespace toy2d {
	class Renderer final
	{
	public:
		Renderer();
		Renderer(std::vector<Vertex> vex, std::vector<std::uint32_t> ind);
		~Renderer();

		void Draw();
		
		void set__Vertices(std::vector<Vertex> vex);
		void set_Index(std::vector<std::uint32_t> ind);

	private:
		int maxFlightCount_ = 2;//˫����
		int curFrame_ = 0;//��ǰ֡
		int uniformCount_ = 2;//uniform��������

		std::vector<Vertex> vertices = {
			Vertex{{-0.5, 0.5, 0}, {1.0, 0.0, 0.0}},
			Vertex{{-0.5, -0.5, 0}, {0.0, 1.0, 0.0}},
			Vertex{{0.5, 0.5, 0}, {0.0, 0.0, 1.0}},
			Vertex{{0.5, -0.5, 0}, {1.0, 1.0, 1.0}},
			//TODO: ����������

		};//��������

		std::vector<std::uint32_t> indices {
			0, 1, 2,
			3, 2, 1
		};//��������

		glm::vec3 UniformColor{ 1, 0.5, 0 };
		glm::mat4 UniformTrans = glm::mat4(1.0f);
		
		void init();//��ʼ��Vulkan
		vk::CommandPool cmdPool_;//������
		std::vector<vk::CommandBuffer> cmdBuf_;//���ڴ洢�����
		std::vector<vk::Semaphore> imageAvaliable_;//���ƻ��������ύ���ź�
		std::vector<vk::Semaphore> imageDrawFinish_;//����ͼ����ʾ���ź�
		std::vector<vk::Fence> cmdAvaliableFence_;//��������CPU��GPU֮��ͬ����դ��

		std::unique_ptr<Buffer> hostVertexBuffer_;//���ض������껺��
		std::unique_ptr<Buffer> deviceVertexBuffer_;
		std::unique_ptr<Buffer> hostIndicesBuffer_;//���ض�����������
		std::unique_ptr<Buffer> deviceIndicesBuffer_;

		std::vector<std::vector<std::unique_ptr<Buffer>>> hostUniformBuffer_;//CPU��Uniformbuffer
		std::vector<std::vector<std::unique_ptr<Buffer>>> deviceUniformBuffer_;//GPU��Uniformbuffer

		vk::DescriptorPool descriptorPool_;
        std::vector<std::vector<vk::DescriptorSet>> sets_;

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
		void allocateSets(int binding_num);
		void updateSets(int binding_num);

		//                  Դ��������Ŀ�껺������������С��Դƫ�ƣ�Ŀ��ƫ��
		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
	};
}

#endif // !RENDERER