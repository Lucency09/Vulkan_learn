#include "Render/Vulkan/include/buffer.hpp"
#include "Render/Vulkan/include/context.hpp"

namespace toy2d {
	Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property)
	{//宽度，使用方式，内存所有者
		createBuffer(size, usage);
		auto info = queryMemoryInfo(property);
		allocateMemory(info);
		bindingMem2Buf();
		this->size = size;
	}

	Buffer::~Buffer()
	{
		Context::GetInstance().get_device().freeMemory(this->memory);
		Context::GetInstance().get_device().destroyBuffer(this->buffer);
	}

	void Buffer::createBuffer(size_t size, vk::BufferUsageFlags usage)
	{
		vk::BufferCreateInfo createInfo;
		createInfo.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive);//共享模式(是否多个命令队列共享)

		this->buffer = Context::GetInstance().get_device().createBuffer(createInfo);
	}

	void Buffer::allocateMemory(MemoryInfo info)
	{
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.setMemoryTypeIndex(info.index)
			.setAllocationSize(info.size);
		this->memory = Context::GetInstance().get_device().allocateMemory(allocInfo);
	}

	Buffer::MemoryInfo Buffer::queryMemoryInfo(vk::MemoryPropertyFlags property)
	{
		//查询当前缓冲区this->buffer需要占用当前逻辑设备中多宽的内存，存在内存对齐问题可能导致其之与上诉size不一致
		MemoryInfo info;
		auto requirements = Context::GetInstance().get_device().getBufferMemoryRequirements(this->buffer);
		this->requireSize = requirements.size;
		info.size = requirements.size;

		auto properties = Context::GetInstance().get_phyDevice().getMemoryProperties();//当前硬件设备支持的内存类型列表
		info.index = Context::GetInstance().QueryBufferMemTypeIndex(requirements.memoryTypeBits, property);
		return info;
	}

	void Buffer::bindingMem2Buf()
	{
		Context::GetInstance().get_device().bindBufferMemory(this->buffer, memory, 0);
	}
};