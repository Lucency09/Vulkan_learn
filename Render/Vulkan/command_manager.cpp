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
        Context::GetInstance().get_device().resetCommandPool(this->pool_);//���û����
    }

    vk::CommandPool CommandManager::createCommandPool() 
    {
        auto& ctx = Context::GetInstance();

        vk::CommandPoolCreateInfo createInfo;

        createInfo.setQueueFamilyIndex(ctx.get_queueFamilyIndices().graphicsQueue.value())//���øû�����з���Ļ����������ύ��graphicsQueueͼ�������
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//��ÿһ��CommandBuffer���Ե���reset

        return ctx.get_device().createCommandPool(createInfo);
    }

    std::vector<vk::CommandBuffer> CommandManager::CreateCommandBuffers(std::uint32_t count) 
    {
        auto& ctx = Context::GetInstance();

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(this->pool_)//���ô�this->pool_�з���
            .setCommandBufferCount(count)//����count��Buffer
            .setLevel(vk::CommandBufferLevel::ePrimary);//����ΪGPUֱ��ִ��

        return ctx.get_device().allocateCommandBuffers(allocInfo);//����vector������setCommandBufferCount����
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
