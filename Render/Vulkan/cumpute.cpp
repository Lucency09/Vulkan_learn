#include "Render/Vulkan/include/cumpute.hpp"
#include "Render/Vulkan/include/context.hpp"

toy2d::Cumpute::Cumpute()
{
	//this->instance = Context::get_instance();
	this->device = Context::GetInstance().get_device();
	//this->shadersource = 
}

toy2d::Cumpute::Cumpute(const std::string& shaderpath)
{
	this->device = Context::GetInstance().get_device();
	this->shadersource = toy2d::Read_spv_File(shaderpath);
	this->shadermodule = this->createShaderModule(this->device, this->shadersource);
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