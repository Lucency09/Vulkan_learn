#include "render_process.hpp"
#include "context.hpp"

namespace toy2d
{
	void RenderProcess::InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height)
	{
		this->shader.Init(vertexSource, fragSource);//��ʼ��shader����
		vk::GraphicsPipelineCreateInfo createinfo;
	
		//1. ��������
		vk::PipelineVertexInputStateCreateInfo inputState;
		auto attribute = Vertex::GetAttribute();
		auto binding = Vertex::GetBinding();
		//inputState.setVertexBindingDescriptions(binding)//���Ͷ������ݵĴ洢��ʽ
		//	.setVertexAttributeDescriptions(attribute);
		createinfo.setPVertexInputState(&inputState);

		//2. ͼԪװ�䣿	Vertex Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		inputAsm.setPrimitiveRestartEnable(false)//��ʱ����
			.setTopology(vk::PrimitiveTopology::eTriangleList);//����ͼԪ����,��������Ϊ������
		createinfo.setPInputAssemblyState(&inputAsm);

		//3. ����Shader
		std::vector<vk::PipelineShaderStageCreateInfo> stages = this->shader.GetStage();
		createinfo.setStages(stages);

		//4. viewport
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);//���óߴ�
		viewportState.setViewports(viewport);
		vk::Rect2D rect({ 0, 0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
		//				ƫ����(�ӵ�0�е�0�п�ʼ)		���γߴ�
		viewportState.setScissors(rect);
		createinfo.setPViewportState(&viewportState);

		//5. ��դ������	Rasterization
		vk::PipelineRasterizationStateCreateInfo rastInfo;
		rastInfo.setRasterizerDiscardEnable(false)//�Ƿ�������դ�����
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eClockwise)//��������
			.setPolygonMode(vk::PolygonMode::eFill)//�������ģʽ,��������Ϊȫ�����
			.setLineWidth(1);//���ñ��߿��
		createinfo.setPRasterizationState(&rastInfo);

		//6. ���ز���	multisampel
		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)//�رն��ز���
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);//���ù�դ���׶β�������Ϊ1
		createinfo.setPMultisampleState(&multisample);

		//7. ��Ȳ��ԡ�ģ�����(����)	depth test  stencil test

		//8. ɫ�ʻ��(͸�����˾�֮��)	color blending
		vk::PipelineColorBlendStateCreateInfo blend;
		vk::PipelineColorBlendAttachmentState attachs;

		attachs.setBlendEnable(false)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA |
							vk::ColorComponentFlagBits::eB |
							vk::ColorComponentFlagBits::eG |
							vk::ColorComponentFlagBits::eR);//�����ļ���ȡ��ʽ������4��ͨ����Ҫ��
		blend.setLogicOpEnable(false)//�ر���ɫ�ڻ�
			.setAttachments(attachs);
		createinfo.setPColorBlendState(&blend);

		//9. renderpass and layout
		createinfo.setRenderPass(this->renderPass);
		createinfo.setLayout(this->layout);

		//ʵ�ʴ�����Ⱦ����
		vk::ResultValue<vk::Pipeline> result = Context::GetInstance().get_device().createGraphicsPipeline(nullptr, createinfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("create graphics pipeline failed");
		}
		this->pipeline = result.value;
	}

	void RenderProcess::InitLayout()
	{
		vk::PipelineLayoutCreateInfo createinfo;
		this->layout = Context::GetInstance().get_device().createPipelineLayout(createinfo);
	}

	void RenderProcess::InitRenderPass()
	{
		vk::RenderPassCreateInfo createInfo;
		vk::AttachmentDescription attachDesc;//�������ÿ��Խ�����ߵ�������������
		attachDesc.setFormat(Context::GetInstance().get_swapchain()->get_info().format.format)
			.setInitialLayout(vk::ImageLayout::eUndefined)//������Ⱦ����ʱ�Ĳ���,�������õ���δ����
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)//�߳���Ⱦ����ʱ�Ĳ��֣��������ó���ʾ����
			.setLoadOp(vk::AttachmentLoadOp::eClear)//�ռ��ؽ�����ʱ�����������ɫ
			.setStoreOp(vk::AttachmentStoreOp::eStore)//������ɺ��������(eDontCar�ǲ������Ƿ񱣴�)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)//����ģ�建��
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)//����ģ����Խ���Ƿ񱣴�
			.setSamples(vk::SampleCountFlagBits::e1);//���������ʽ
		createInfo.setAttachments(attachDesc);

		vk::AttachmentReference reference;//��������
		reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal)//����Ϊ��ɫ���͵�����
			.setAttachment(0);//ʹ�õ�0������(�Ϸ�ֻ������һ��������attachDesc,����������һ������,�ú�������ָ���ڼ���)
		vk::SubpassDescription subpass;
		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)//������ͼ�����͵���Ⱦ������
			.setColorAttachments(reference);
		createInfo.setSubpasses(subpass);

		vk::SubpassDependency dependency;//���ж��subpassʱ����ָ��ִ��˳��,����֮��vulkan�л���һЩĬ�ϵ���Ⱦ������
		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)//����ִ���ⲿ��subpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)//Ȼ��ָ���Ϸ������� subpass
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)//������ɫд��
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)//ִ�н������������ɫ������
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);//ִ�н������������ɫ������
		//�Ϸ�Dst��ӦDst,Src��ӦSrc
		createInfo.setDependencies(dependency);

		this->renderPass = Context::GetInstance().get_device().createRenderPass(createInfo);//���մ�����Ⱦ����

		
	}

	vk::RenderPass& RenderProcess::get_renderPass()
	{
		return renderPass;
	}

	vk::Pipeline& RenderProcess::get_pipeline()
	{
		return this->pipeline;
	}

	RenderProcess::~RenderProcess()
	{
		auto& device = Context::GetInstance().get_device();
		
		
		device.destroyRenderPass(this->renderPass);
		device.destroyPipelineLayout(this->layout);
		device.destroyPipeline(this->pipeline);
	}
}