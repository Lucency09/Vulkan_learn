#pragma once

#include "vulkan/vulkan.hpp"
#include "shader.hpp"

#ifndef RENDER_PROCESS
#define RENDER_PROCESS

namespace toy2d
{
	class RenderProcess final
	{
	public:
		void InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height);
		void InitLayout();

		vk::PipelineLayout& get_layout();
		vk::RenderPass& get_renderPass();
		vk::Pipeline& get_pipeline();
		vk::DescriptorSetLayout& get_setLayout();
		vk::PushConstantRange GetPushConstantRange() const;//用于指定push常量布局
		

		~RenderProcess();
		RenderProcess();

	private:
		vk::Pipeline graphicsPipeline = nullptr;
		vk::PipelineLayout layout = nullptr;//涉及uniform数据
		vk::RenderPass renderPass = nullptr;//渲染流程
		vk::DescriptorSetLayout setLayout = nullptr;

		vkShader shader;
		vk::PipelineLayout createLayout();//用于指定uniform布局
		vk::DescriptorSetLayout createSetLayout();
		vk::Pipeline createGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
		void InitRenderPass();
	};
}
#endif // !RENDER_PROCESS