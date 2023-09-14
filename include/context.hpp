#pragma once

#include "vulkan/vulkan.hpp"
#include <iostream>

namespace toy2d
{
	class Context final
	{
	public:
		static void Init();
		static void Quit();
		static Context& GetInstance();
		vk::Instance get_instance();//·µ»ØVulkan¾ä±ú
		~Context();
		
	private:
		Context();
		static Context* instance_;
		vk::Instance instance = nullptr;
		
	};
}