#include "render_process.hpp"
#include "context.hpp"

namespace toy2d
{
	void RenderProcess::InitPipeLine(const std::string& vertexSource, const std::string& fragSource, int width, int height)
	{
		this->shader.Init(vertexSource, fragSource);//��ʼ��shader����
		vk::GraphicsPipelineCreateInfo createinfo;
	
		//��������
		vk::PipelineVertexInputStateCreateInfo inputState;
		createinfo.setPVertexInputState(&inputState);

		//ͼԪװ�䣿	Vertex Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		inputAsm.setPrimitiveRestartEnable(false)//��ʱ����
			.setTopology(vk::PrimitiveTopology::eTriangleList);//����ͼԪ����,��������Ϊ������
		createinfo.setPInputAssemblyState(&inputAsm);

		//����Shader
		std::vector<vk::PipelineShaderStageCreateInfo> stages = this->shader.GetStage();
		createinfo.setStages(stages);

		// 4. viewport
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, width, height, 0, 1);//���óߴ�
		viewportState.setViewports(viewport);
		vk::Rect2D rect({ 0, 0 }, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
		//				ƫ����(�ӵ�0�е�0�п�ʼ)		���γߴ�
		viewportState.setScissors(rect);
		createinfo.setPViewportState(&viewportState);

		//��դ������	Rasterization
		vk::PipelineRasterizationStateCreateInfo rastInfo;
		rastInfo.setRasterizerDiscardEnable(false)//�Ƿ�������դ�����
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)//������ʱ��Ϊ����
			.setPolygonMode(vk::PolygonMode::eFill)//�������ģʽ,��������Ϊȫ�����
			.setLineWidth(1);//���ñ��߿��
		createinfo.setPRasterizationState(&rastInfo);

		//���ز���	multisampel
		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.setSampleShadingEnable(false)//�رն��ز���
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);//���ù�դ���׶β�������Ϊ1
		createinfo.setPMultisampleState(&multisample);

		//��Ȳ��ԡ�ģ�����(����)	depth test  stencil test

		//ɫ�ʻ��(͸�����˾�֮��)	color blending
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

		//ʵ�ʴ�����Ⱦ����
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