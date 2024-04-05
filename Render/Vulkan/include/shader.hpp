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

		std::vector<vk::PipelineShaderStageCreateInfo> GetStage();//返回shader程序

	private:
		vk::ShaderModule vertexModule;//顶点着色器
		vk::ShaderModule fragmentModule;//片段着色器
		std::vector<vk::PipelineShaderStageCreateInfo> stage_;//shader程序

		void initStage();
	};
}
#endif // !SHADER