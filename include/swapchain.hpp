#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d
{
	class Swapchain final
	{
	public:
		Swapchain();
		~Swapchain();
	private:
		vk::SwapchainKHR swapchain;
	};
}