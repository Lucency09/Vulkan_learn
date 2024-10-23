#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {
	class Cumpute final {
	public:
		Cumpute();
		Cumpute(const std::string& shaderpath);
		~Cumpute() = default;

		//void set_

	private:
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Queue queue;
		vk::ShaderModule shadermodule;
		vk::PipelineLayout piplinelayout;
		vk::Pipeline pipeline;
		std::string shadersource;

		vk::ShaderModule createShaderModule(vk::Device device, const std::string& shadersource);

		//void init();
	};

}