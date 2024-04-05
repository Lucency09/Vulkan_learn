#include "Render/Vulkan/include/buffer.hpp"
#include "Render/Vulkan/include/context.hpp"

namespace toy2d {
	Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property)
	{//��ȣ�ʹ�÷�ʽ���ڴ�������
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
			.setSharingMode(vk::SharingMode::eExclusive);//����ģʽ(�Ƿ���������й���)

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
		//��ѯ��ǰ������this->buffer��Ҫռ�õ�ǰ�߼��豸�ж����ڴ棬�����ڴ����������ܵ�����֮������size��һ��
		MemoryInfo info;
		auto requirements = Context::GetInstance().get_device().getBufferMemoryRequirements(this->buffer);
		this->requireSize = requirements.size;
		info.size = requirements.size;

		auto properties = Context::GetInstance().get_phyDevice().getMemoryProperties();//��ǰӲ���豸֧�ֵ��ڴ������б�
		for (int i = 0; i < properties.memoryTypeCount; i++) 
		{
			if ((1 << i) & requirements.memoryTypeBits &&//��λֵΪ1����ʾӲ��֧��������͵��ڴ�
				properties.memoryTypes[i].propertyFlags & property) //���ڴ����;�����Ҫָ�����ڴ�����(���property)
			{
				info.index = i;
				break;
			}
		}

		return info;
	}

	void Buffer::bindingMem2Buf()
	{
		Context::GetInstance().get_device().bindBufferMemory(this->buffer, memory, 0);
	}
};