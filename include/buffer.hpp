#pragma once

#include "vulkan/vulkan.hpp"


namespace toy2d {

    class Buffer final {
    public:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        void* map;
        size_t size;
        size_t requireSize;

        Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
        ~Buffer();

    private:
        struct MemoryInfo final {
            size_t size;
            uint32_t index;
        };

        void createBuffer(size_t size, vk::BufferUsageFlags usage);//创建缓冲区
        void allocateMemory(MemoryInfo info);//分配一块内存
        void bindingMem2Buf();//将buffer和内存绑定
        
        //找到指定内存类型property在当前硬件设备中的所需宽度及编号
        MemoryInfo queryMemoryInfo(vk::MemoryPropertyFlags property);
    };

}
