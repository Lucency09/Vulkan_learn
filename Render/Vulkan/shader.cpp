#include "Render/Vulkan/include/shader.hpp"
#include "Render/Vulkan/include/context.hpp"

toy2d::vkShader::vkShader(const std::string& vertexSource, const std::string& fragSource)
{
    this->Init(vertexSource, fragSource);
}
toy2d::vkShader::vkShader()
{
    return;
}

toy2d::vkShader::~vkShader()
{
    vk::Device& device = Context::GetInstance().get_device();
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);
}

void toy2d::vkShader::Init(const std::string& vertexSource, const std::string& fragSource)
{
    vk::ShaderModuleCreateInfo createInfo;
    //����������ɫ��
    createInfo.codeSize = vertexSource.size();
    createInfo.pCode = (uint32_t*)vertexSource.data();
    this->vertexModule = Context::GetInstance().get_device().createShaderModule(createInfo);
    //����Ƭ����ɫ��
    createInfo.codeSize = fragSource.size();
    createInfo.pCode = (uint32_t*)fragSource.data();
    this->fragmentModule = Context::GetInstance().get_device().createShaderModule(createInfo);

    this->initStage();
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

std::vector<vk::PipelineShaderStageCreateInfo> toy2d::vkShader::GetStage()
{
    return this->stage_;
}

void toy2d::vkShader::initStage()
{
    this->stage_.resize(2);//���ó���
    this->stage_[0].setStage(vk::ShaderStageFlagBits::eVertex)//��������
        .setModule(vertexModule)//����Դ��
        .setPName("main");//����shader������
    this->stage_[1].setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragmentModule)
        .setPName("main");
}
