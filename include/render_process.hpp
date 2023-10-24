#pragma once

#include "vulkan/vulkan.hpp"
#include "shader.hpp"

namespace toy2d
{
	class RenderProcess final
	{
	public:
		void InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height);
		void InitLayout();
		void InitRenderPass();
		~RenderProcess();

	private:
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;//�漰uniform����
		vk::RenderPass renderPass;//��Ⱦ����
		vkShader shader;
		
	};
}