#include "Render/Vulkan/include/cumpute.hpp"
#include "Render/Vulkan/include/context.hpp"

toy2d::Cumpute::Cumpute()
{
	//this->instance = Context::get_instance();
	this->shadersource = toy2d::Read_spv_File("res/Spir-v/dxt_encode.spv");
	this->device = Context::GetInstance().get_device();
	this->queue = this->device.getQueue(0, 0);
	this->shadermodule = this->createShaderModule(this->device, this->shadersource);
	this->descriptorSetLayout = this->createdescriptorSetLayout();
	this->piplinelayout = this->createpiplinelayout();
	this->pipeline = this->createCumputePipline();
}

toy2d::Cumpute::Cumpute(const std::string& shaderpath)
{
	this->shadersource = toy2d::Read_spv_File(shaderpath);
	this->device = Context::GetInstance().get_device();
	this->queue = this->device.getQueue(0, 0);
	this->shadermodule = this->createShaderModule(this->device, this->shadersource);
	this->descriptorSetLayout = this->createdescriptorSetLayout();
	this->piplinelayout = this->createpiplinelayout();
	this->pipeline = this->createCumputePipline();
	std::cout << "Cumpute builded over";
}

void toy2d::Cumpute::bindBuffer(const Buffer& inputbuffer, const Buffer& outputbuffer)
{
	return;
}



vk::ShaderModule toy2d::Cumpute::createShaderModule(vk::Device device, const std::string& shadersource)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shadersource.size();
	createInfo.pCode = (uint32_t*)shadersource.data();

	vk::ShaderModule shaderModuel = device.createShaderModule(createInfo);
	return shaderModuel;
}

vk::DescriptorSetLayout toy2d::Cumpute::createdescriptorSetLayout()
{
	std::array<vk::DescriptorSetLayoutBinding, 2> layoutBindings = {
		vk::DescriptorSetLayoutBinding(
			0, // binding
			vk::DescriptorType::eStorageImage, // descriptor type
			1, // descriptor count
			vk::ShaderStageFlagBits::eCompute // stage flags
		),
		vk::DescriptorSetLayoutBinding(
			1, // binding
			vk::DescriptorType::eStorageImage, // descriptor type
			1, // descriptor count
			vk::ShaderStageFlagBits::eCompute // stage flags
		)
	};
	vk::DescriptorSetLayoutCreateInfo createinfo({}, layoutBindings);
	return this->device.createDescriptorSetLayout(createinfo);
}

vk::PipelineLayout toy2d::Cumpute::createpiplinelayout()
{
	vk::PipelineLayoutCreateInfo piplineLayoutinfo({}, this->descriptorSetLayout);
	return this->device.createPipelineLayout(piplineLayoutinfo);
}

vk::Pipeline toy2d::Cumpute::createCumputePipline()
{
	vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, this->shadermodule, "main");
	vk::ComputePipelineCreateInfo piplineCreateInfo({}, shaderStageCreateInfo, this->piplinelayout);
	return this->device.createComputePipeline({},piplineCreateInfo).value;
}