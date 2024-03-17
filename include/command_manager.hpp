#pragma once

#include "vulkan/vulkan.hpp"

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

    private:
        vk::CommandPool pool_;//������

        vk::CommandPool createCommandPool();
    };

}
#endif // !COMMAND_MANAGER