#pragma once

#include "vulkan/vulkan.hpp"


namespace toy2d {

    class Buffer final {
    public:
        vk::Buffer buffer;//缓冲区的句柄(不存储实际数据)
        vk::DeviceMemory memory;//缓冲区的实际内存，存储实际数据
        size_t size;//缓冲区的大小(读写数据的时候用这个)
        size_t requireSize;//缓冲区的实际内存大小

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
