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

		vk::PipelineLayout& get_layout();
		vk::RenderPass& get_renderPass();
		vk::Pipeline& get_pipeline();
		vk::DescriptorSetLayout& get_setLayout();
		

		~RenderProcess();
		RenderProcess();

	private:
		vk::Pipeline graphicsPipeline = nullptr;
		vk::PipelineLayout layout = nullptr;//�漰uniform����
		vk::RenderPass renderPass = nullptr;//��Ⱦ����
		vk::DescriptorSetLayout setLayout = nullptr;

		vkShader shader;
		vk::PipelineLayout createLayout();//����ָ��uniform����
		vk::DescriptorSetLayout createSetLayout();
		vk::Pipeline createGraphicsPipeline(const std::vector<char>& vertexSource, const std::vector<char>& fragSource);
		void InitRenderPass();
	};
}