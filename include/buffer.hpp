#pragma once

#include "vulkan/vulkan.hpp"


namespace toy2d {

    class Buffer final {
    public:
        vk::Buffer buffer;//�������ľ��(���洢ʵ������)
        vk::DeviceMemory memory;//��������ʵ���ڴ棬�洢ʵ������
        size_t size;//�������Ĵ�С(��д���ݵ�ʱ�������)
        size_t requireSize;//��������ʵ���ڴ��С

        Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
        ~Buffer();

    private:
        struct MemoryInfo final {
            size_t size;
            uint32_t index;
        };

        void createBuffer(size_t size, vk::BufferUsageFlags usage);//����������
        void allocateMemory(MemoryInfo info);//����һ���ڴ�
        void bindingMem2Buf();//��buffer���ڴ��
        
        //�ҵ�ָ���ڴ�����property�ڵ�ǰӲ���豸�е������ȼ����
        MemoryInfo queryMemoryInfo(vk::MemoryPropertyFlags property);
    };

}
