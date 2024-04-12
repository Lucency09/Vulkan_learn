#pragma once

#include "vulkan/vulkan.hpp"

#include <functional> 

#ifndef COMMAND_MANAGER
#define COMMAND_MANAGER

namespace toy2d {

    class CommandManager final 
    {
    public:
        CommandManager();
        ~CommandManager();

        vk::CommandBuffer CreateOneCommandBuffer();
        std::vector<vk::CommandBuffer> CreateCommandBuffers(std::uint32_t count);
        void ResetCmds();
        void FreeCmd(vk::CommandBuffer);

        using RecordCmdFunc = std::function<void(vk::CommandBuffer&)>;
        void ExecuteCmd(vk::Queue queue, RecordCmdFunc func);//向命令队列Queue中提交执行命令RecordCmdFunc

    private:
        vk::CommandPool pool_;//命令缓冲池

        vk::CommandPool createCommandPool();
    };

}
#endif // !COMMAND_MANAGER