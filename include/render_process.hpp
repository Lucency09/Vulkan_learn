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
		vk::PipelineLayout layout;//涉及uniform数据
		vk::RenderPass renderPass;//渲染流程
		vkShader shader;
		
	};
}