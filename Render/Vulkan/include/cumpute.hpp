#pragma once


#include "vulkan/vulkan.hpp"
#include "Render/Vulkan/include/texture.hpp"

#ifndef CUMPUTE 
#define CUMPUTE
namespace toy2d {
	class Cumpute final {
	public:
		Cumpute();
		Cumpute(const std::string& shaderpath);
		~Cumpute() = default;

		void run_comput(const Texture& inputtexture, Buffer& outputbuffer);

	private:
		vk::Instance instance;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Queue queue;
		vk::DescriptorSetLayout descriptorSetLayout;
		vk::ShaderModule shadermodule;
		vk::PipelineLayout piplinelayout;
		vk::Pipeline pipeline;
		vk::CommandPool commandPool;
		vk::Sampler sampler;
		std::string shadersource;

		vk::ShaderModule createShaderModule(vk::Device device, const std::string& shadersource);
		vk::PipelineLayout createpiplinelayout();
		vk::Pipeline createCumputePipline();
		vk::DescriptorSetLayout createdescriptorSetLayout();
		vk::CommandPool createCommandPool();
		vk::Sampler createSampler();

		//void init();
		
	};

}
#endif // !CUMPUTE