#include "shader.hpp"
#include "context.hpp"

toy2d::vkShader::vkShader(const std::string& vertexSource, const std::string& fragSource)
{
    vk::ShaderModuleCreateInfo createInfo;
    //创建顶点着色器
    createInfo.codeSize = vertexSource.size();
    createInfo.pCode = (uint32_t*)vertexSource.data();
    vertexModule = Context::GetInstance().get_device().createShaderModule(createInfo);
    //创建片段着色器
    createInfo.codeSize = fragSource.size();
    createInfo.pCode = (uint32_t*)fragSource.data();
    fragmentModule = Context::GetInstance().get_device().createShaderModule(createInfo);
}

toy2d::vkShader::~vkShader()
{
    vk::Device& device = Context::GetInstance().get_device();
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);
}

void toy2d::vkShader::Init()
{
}

void toy2d::vkShader::Quit()
{
}

vk::ShaderModule& toy2d::vkShader::get_vertexModule()
{
	return this->vertexModule;
}

vk::ShaderModule& toy2d::vkShader::get_fragmentModule()
{
	return this->fragmentModule;
}
