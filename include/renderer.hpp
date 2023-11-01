#pragma once

#include <vulkan/vulkan.hpp>

namespace toy2d
{
	class Renderer final
	{
	public:
		Renderer();
		~Renderer();
	private:
		vk::CommandPool cmdPool;
		void initCmdPool();

	};
}