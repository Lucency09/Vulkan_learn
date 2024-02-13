#include "render_process.hpp"
#include "context.hpp"

namespace toy2d
{
	void RenderProcess::InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height)
	{
		this->shader.Init(vertexSource, fragSource);//初始化shader程序
		vk::GraphicsPipelineCreateInfo createinfo;
	
		//1. 顶点输入
		vk::PipelineVertexInputStateCreateInfo inputState;
		auto attribute = Vertex::GetAttribute();
		auto binding = Vertex::GetBinding();
		inputState.setVertexBindingDescriptions(binding)//解释顶点数据的存储格式
			.setVertexAttributeDescriptions(attribute);
		createinfo.setPVertexInputState(&inputState);

		//2. 图元装配？	Vertex Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		inputAsm.setPrimitiveRestartEnable(false)//暂时不用
			.setTopology(vk::PrimitiveTopology::eTriangleList);//设置图元类型,这里设置为三角形
		createinfo.setPInputAssemblyState(&inputAsm);

		//3. 设置Shader
		std::vector<vk::PipelineShaderStageCreateInfo> stages = this->shader.GetStage();
		createinfo.setStages(stages);

		//4. viewport
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);//设置尺寸
		viewportState.setViewports(viewport);
		vk::Rect2D rect({ 0, 0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
		//				偏移量(从第0行第0列开始)		矩形尺寸
		viewportState.setScissors(rect);
		createinfo.setPViewportState(&viewportState);

		//5. 光栅化处理	Rasterization
		vk::PipelineRasterizationStateCreateInfo rastInfo;
		rastInfo.setRasterizerDiscardEnable(false)//是否抛弃光栅化结果
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)//设置正面
			.setPolygonMode(vk::PolygonMode::eFill)//设置填充模式,这里设置为全部填充
			.setLineWidth(1);//设置边线宽度
		createinfo.setPRasterizationState(&rastInfo);

		//6. 多重采样	multisampel
		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)//关闭多重采样
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);//设置光栅化阶段采样级别为1
		createinfo.setPMultisampleState(&multisample);

		//7. 深度测试、模板测试(跳过)	depth test  stencil test

		//8. 色彩混合(透明、滤镜之类)	color blending
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

		//9. renderpass and layout
		createinfo.setRenderPass(this->renderPass)
					.setLayout(this->layout);

		//实际创建渲染管线
		vk::ResultValue<vk::Pipeline> result = Context::GetInstance().get_device().createGraphicsPipeline(nullptr, createinfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create graphics pipeline failed");
		}
		this->graphicsPipeline = result.value;
	}

	void RenderProcess::InitRenderPass()
	{
		vk::RenderPassCreateInfo createInfo;
		vk::AttachmentDescription attachDesc;//用于设置可以进入管线的纹理附件的描述
		attachDesc.setFormat(Context::GetInstance().get_swapchain()->get_info().format.format)
			.setInitialLayout(vk::ImageLayout::eUndefined)//进入渲染流程时的布局,这里设置的是未定义
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)//走出渲染流程时的布局，这里设置成显示附件
			.setLoadOp(vk::AttachmentLoadOp::eClear)//刚加载进来的时候清空所有颜色
			.setStoreOp(vk::AttachmentStoreOp::eStore)//绘制完成后存下数据(eDontCar是不关心是否保存)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)//设置模板缓冲
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)//设置模板测试结果是否保存
			.setSamples(vk::SampleCountFlagBits::e1);//纹理采样方式
		createInfo.setAttachments(attachDesc);

		vk::AttachmentReference reference;//纹理引用
		reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal)//设置为颜色类型的纹理
			.setAttachment(0);//使用第0个纹理(上方只创建了一个纹理附件attachDesc,若其类型是一个数组,该函数用于指定第几个)
		vk::SubpassDescription subpass;
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)//设置在图像类型的渲染管线上
			.setColorAttachments(reference);
		createInfo.setSubpasses(subpass);

		vk::SubpassDependency dependency;//当有多个subpass时用于指定执行顺序,除此之外vulkan中还有一些默认的渲染子流程
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)//首先执行外部的subpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)//然后指向上方创建的 subpass
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)//允许颜色写入
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)//执行结束后输出到颜色附件上
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);//执行结束后输出到颜色附件上
		//上方Dst对应Dst,Src对应Src
		createInfo.setDependencies(dependency);

		this->renderPass = Context::GetInstance().get_device().createRenderPass(createInfo);//最终创建渲染流程

	}

	vk::PipelineLayout& RenderProcess::get_layout()
	{
		return this->layout;
	}

	vk::RenderPass& RenderProcess::get_renderPass()
	{
		return this->renderPass;
	}

	vk::Pipeline& RenderProcess::get_pipeline()
	{
		return this->graphicsPipeline;
	}

	vk::DescriptorSetLayout& RenderProcess::get_setLayout()
	{
		return this->setLayout;
	}

	RenderProcess::~RenderProcess()
	{
		auto& device = Context::GetInstance().get_device();
		
		device.destroyDescriptorSetLayout(this->setLayout);
		device.destroyRenderPass(this->renderPass);
		device.destroyPipelineLayout(this->layout);
		device.destroyPipeline(this->graphicsPipeline);
	}

	RenderProcess::RenderProcess()
	{
		this->setLayout = createSetLayout();
		this->layout = createLayout();
		this->InitRenderPass();
	}

	vk::PipelineLayout RenderProcess::createLayout()
	{
		vk::PipelineLayoutCreateInfo createInfo;
		createInfo.setSetLayouts(this->setLayout);

		return Context::GetInstance().get_device().createPipelineLayout(createInfo);
	}

	vk::DescriptorSetLayout RenderProcess::createSetLayout()
	{
		vk::DescriptorSetLayoutCreateInfo createInfo;
		auto binding = Uniform::GetBinding();
		createInfo.setBindings(binding);

		return Context::GetInstance().get_device().createDescriptorSetLayout(createInfo);
	}

}