#include "Render/Vulkan/include/command_manager.hpp"
#include "Render/Vulkan/include/context.hpp"

namespace toy2d {

    CommandManager::CommandManager() 
    {
        this->pool_ = createCommandPool();
    }

    CommandManager::~CommandManager() 
    {
        auto& ctx = Context::GetInstance();
        ctx.get_device().destroyCommandPool(this->pool_);
    }

    void CommandManager::ResetCmds() 
    {
        Context::GetInstance().get_device().resetCommandPool(this->pool_);//重置缓冲池
    }

    vk::CommandPool CommandManager::createCommandPool() 
    {
        auto& ctx = Context::GetInstance();

        vk::CommandPoolCreateInfo createInfo;

        createInfo.setQueueFamilyIndex(ctx.get_queueFamilyIndices().graphicsQueue.value())//设置该缓冲池中分配的缓冲区必须提交至graphicsQueue图像队列上
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//让每一个CommandBuffer可以单独reset

        return ctx.get_device().createCommandPool(createInfo);
    }

    std::vector<vk::CommandBuffer> CommandManager::CreateCommandBuffers(std::uint32_t count) 
    {
        auto& ctx = Context::GetInstance();

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(this->pool_)//设置从this->pool_中分配
            .setCommandBufferCount(count)//分配count个Buffer
            .setLevel(vk::CommandBufferLevel::ePrimary);//设置为GPU直接执行

        return ctx.get_device().allocateCommandBuffers(allocInfo);//返回vector长度由setCommandBufferCount决定
    }

    vk::CommandBuffer CommandManager::CreateOneCommandBuffer() 
    {
        return CreateCommandBuffers(1)[0];
    }

    void CommandManager::FreeCmd(vk::CommandBuffer buf) 
    {
        Context::GetInstance().get_device().freeCommandBuffers(pool_, buf);
    }

}
