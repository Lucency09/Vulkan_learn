#include "render_process.hpp"
#include "context.hpp"

namespace toy2d
{
	void RenderProcess::InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height)
	{
		this->shader.Init(vertexSource, fragSource);//初始化shader程序
		vk::GraphicsPipelineCreateInfo createinfo;
	
		//顶点输入
		vk::PipelineVertexInputStateCreateInfo inputState;
		createinfo.setPVertexInputState(&inputState);

		//图元装配？	Vertex Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		inputAsm.setPrimitiveRestartEnable(false)//暂时不用
			.setTopology(vk::PrimitiveTopology::eTriangleList);//设置图元类型,这里设置为三角形
		createinfo.setPInputAssemblyState(&inputAsm);

		//设置Shader
		std::vector<vk::PipelineShaderStageCreateInfo> stages = this->shader.GetStage();
		createinfo.setStages(stages);

		// 4. viewport
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);//设置尺寸
		viewportState.setViewports(viewport);
		vk::Rect2D rect({ 0, 0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
		//				偏移量(从第0行第0列开始)		矩形尺寸
		viewportState.setScissors(rect);
		createinfo.setPViewportState(&viewportState);

		//光栅化处理	Rasterization
		vk::PipelineRasterizationStateCreateInfo rastInfo;
		rastInfo.setRasterizerDiscardEnable(false)//是否抛弃光栅化结果
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)//设置逆时针为正面
			.setPolygonMode(vk::PolygonMode::eFill)//设置填充模式,这里设置为全部填充
			.setLineWidth(1);//设置边线宽度
		createinfo.setPRasterizationState(&rastInfo);

		//多重采样	multisampel
		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)//关闭多重采样
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);//设置光栅化阶段采样级别为1
		createinfo.setPMultisampleState(&multisample);

		//深度测试、模板测试(跳过)	depth test  stencil test

		//色彩混合(透明、滤镜之类)	color blending
		vk::PipelineColorBlendStateCreateInfo blend;
		vk::PipelineColorBlendAttachmentState attachs;

		attachs.setBlendEnable(false)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA |
							vk::ColorComponentFlagBits::eB |
							vk::ColorComponentFlagBits::eG |
							vk::ColorComponentFlagBits::eR);//纹理文件读取方式，这里4个通道都要读
		blend.setLogicOpEnable(false)//关闭颜色融混
			.setAttachments(attachs);

		createinfo.setPColorBlendState(&blend);

		//实际创建渲染管线
		vk::ResultValue<vk::Pipeline> result = Context::GetInstance().get_device().createGraphicsPipeline(nullptr, createinfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create graphics pipeline failed");
		}
		this->pipeline = result.value;
	}
	RenderProcess::~RenderProcess()
	{
		auto& device = Context::GetInstance().get_device();
		device.destroyPipeline(pipeline);
	}
}