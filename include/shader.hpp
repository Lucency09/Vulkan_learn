#pragma once
#include "vulkan/vulkan.hpp"

#include <iostream>


namespace toy2d
{
	class vkShader final
	{
	public:
		vkShader(const std::string& vertexSource, const std::string& fragSource);
		~vkShader();

		void Init();
		void Quit();

		vk::ShaderModule& get_vertexModule();
		vk::ShaderModule& get_fragmentModule();

	private:
		vk::ShaderModule vertexModule;//������ɫ��
		vk::ShaderModule fragmentModule;//Ƭ����ɫ��
	};
}