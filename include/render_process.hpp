#pragma once

#include "vulkan/vulkan.hpp"
#include "shader.hpp"

namespace toy2d
{
	class RenderProcess final
	{
	public:
		void InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height);
		~RenderProcess();

	private:
		vk::Pipeline pipeline;
		vkShader shader;
		
	};
}