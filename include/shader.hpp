#pragma once
#include "vulkan/vulkan.hpp"

#include <iostream>

#ifndef SHADER
#define SHADER

namespace toy2d
{
	class vkShader final
	{
	public:
		vkShader();
		vkShader(const std::string& vertexSource, const std::string& fragSource);
		~vkShader();

		void Init(const std::string& vertexSource, const std::string& fragSource);
		void Quit();

		vk::ShaderModule& get_vertexModule();
		vk::ShaderModule& get_fragmentModule();

		std::vector<vk::PipelineShaderStageCreateInfo> GetStage();//����shader����

	private:
		vk::ShaderModule vertexModule;//������ɫ��
		vk::ShaderModule fragmentModule;//Ƭ����ɫ��
		std::vector<vk::PipelineShaderStageCreateInfo> stage_;//shader����

		void initStage();
	};
}
#endif // !SHADER