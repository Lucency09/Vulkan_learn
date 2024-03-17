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
		int maxFlightCount_ = 2;//双缓冲
		int curFrame_ = 0;//当前帧
		int uniformCount_ = 2;//uniform变量计数

		std::vector<Vertex> vertices = {
			Vertex{{-0.5, 0.5, 0}, {1.0, 0.0, 0.0}},
			Vertex{{-0.5, -0.5, 0}, {0.0, 1.0, 0.0}},
			Vertex{{0.5, 0.5, 0}, {0.0, 0.0, 1.0}},
			Vertex{{0.5, -0.5, 0}, {1.0, 1.0, 1.0}},
			//TODO: 立方体坐标

		};//顶点坐标

		std::vector<std::uint32_t> indices {
			0, 1, 2,
			3, 2, 1
		};//顶点索引

		glm::vec3 UniformColor{ 1, 0.5, 0 };
		glm::mat4 UniformTrans = glm::mat4(1.0f);
		
		void init();//初始化Vulkan
		vk::CommandPool cmdPool_;//已弃用
		std::vector<vk::CommandBuffer> cmdBuf_;//用于存储命令缓冲
		std::vector<vk::Semaphore> imageAvaliable_;//控制绘制命令提交的信号
		std::vector<vk::Semaphore> imageDrawFinish_;//控制图像显示的信号
		std::vector<vk::Fence> cmdAvaliableFence_;//用于设置CPU和GPU之间同步的栅栏

		std::unique_ptr<Buffer> hostVertexBuffer_;//本地顶点坐标缓冲
		std::unique_ptr<Buffer> deviceVertexBuffer_;
		std::unique_ptr<Buffer> hostIndicesBuffer_;//本地顶点索引缓冲
		std::unique_ptr<Buffer> deviceIndicesBuffer_;

		std::vector<std::vector<std::unique_ptr<Buffer>>> hostUniformBuffer_;//CPU的Uniformbuffer
		std::vector<std::vector<std::unique_ptr<Buffer>>> deviceUniformBuffer_;//GPU的Uniformbuffer

		vk::DescriptorPool descriptorPool_;
        std::vector<std::vector<vk::DescriptorSet>> sets_;

		void initCmdPool();//已弃用，功能转移到createCmdBuffers()
		void allocCmdBuf();//已弃用，功能转移到createCmdBuffers()

		void createFence();//创建Fence对象,其初始状态为非信号态
		void createSemaphores();//创建Semaphore对象
		void createCmdBuffers();
		void createVertexBuffer();
		void createIndicesBuffer();
		
		void createBuffer();//创建verticesBuffer和indicesBuffer
		void updateBuffer();//更新verticesBuffer和indicesBuffer
		void createUniformBuffers(); //创建Uniform数据传输缓冲
		void updateVertexData();//将顶点数据从CPU传输到GPU
		void updateIndicesData();//将索引数据从CPU传输到GPU
		void updateUniformData();//向Uniform缓冲中传入数据
		void createDescriptorPool();
		void allocateSets(int binding_num);
		void updateSets(int binding_num);

		//                  源缓冲区，目标缓冲区，拷贝大小，源偏移，目标偏移
		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
	};
}

#endif // !RENDERER