#include "renderer.hpp"
#include "context.hpp"


toy2d::Renderer::Renderer()
{
	this->initCmdPool();
	this->allocCmdBuf();
    this->createSems();
    this->createFence();
}

toy2d::Renderer::~Renderer()
{
	auto& device = Context::GetInstance().get_device();

	device.freeCommandBuffers(this->cmdPool_,this->cmdBuf_);
	device.destroyCommandPool(this->cmdPool_);
    device.destroySemaphore(this->imageAvaliable_);
    device.destroySemaphore(this->imageDrawFinish_);
    device.destroyFence(this->cmdAvaliableFence_);
}

void toy2d::Renderer::initCmdPool()
{
	vk::CommandPoolCreateInfo createInfo;
	createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//��ÿһ��CommandBuffer���Ե���reset
	this->cmdPool_ = Context::GetInstance().get_device().createCommandPool(createInfo);
}

void toy2d::Renderer::allocCmdBuf()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(this->cmdPool_)//���ô�this->cmdPool_�з���
		.setCommandBufferCount(1)//����1��Buffer
		.setLevel(vk::CommandBufferLevel::ePrimary);//����ΪGPUֱ��ִ��

	this->cmdBuf_ = Context::GetInstance().get_device().allocateCommandBuffers(allocInfo)[0];
}

void toy2d::Renderer::createSems()
{
    vk::SemaphoreCreateInfo createInfo;

    imageAvaliable_ = Context::GetInstance().get_device().createSemaphore(createInfo);
    imageDrawFinish_ = Context::GetInstance().get_device().createSemaphore(createInfo);
}

void toy2d::Renderer::createFence()
{
    vk::FenceCreateInfo createInfo;
    cmdAvaliableFence_ = Context::GetInstance().get_device().createFence(createInfo);
}

void toy2d::Renderer::Render()
{
	auto& device = Context::GetInstance().get_device();
	auto& renderProcess = Context::GetInstance().get_render_process();
	auto& swapchain = Context::GetInstance().get_swapchain();

	//��ѯ��һ�����Ա����Ƶ�image
	auto result = device.acquireNextImageKHR(Context::GetInstance().get_swapchain()->get_swapchain()
												, std::numeric_limits<uint64_t>::max());//����Ϊ���޵ȴ�
	if (result.result != vk::Result::eSuccess)
	{
		std::cout << "acquire next image failed!" << std::endl;
	}

	auto imageIndex = result.value;

	//��ʼ��commandbuffer���������
	this->cmdBuf_.reset();//����Buffer
	vk::CommandBufferBeginInfo begin;
	begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);//������������Ϊʹ��һ�ξ�����
	this->cmdBuf_.begin(begin);
    cmdBuf_.begin(begin); 
    {
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue clearValue;
        clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

        area.setOffset({ 0, 0 })
            .setExtent(swapchain->get_info().imageExtent);//������Ļ��С����swapchain�л�ȡ

        renderPassBegin.setRenderPass(renderProcess.get_renderPass())//������Ⱦ���̣���Context����
            .setRenderArea(area)//������������Ļ�ϻ���
            .setFramebuffer(swapchain->get_framebuffers()[imageIndex])//�������ĸ�framebuffer�ϻ��ƣ���swapchain�л�ȡ 
            .setClearValues(clearValue);//������ʲô��ɫ�����framebuffer(��Ϊ��render_process��setLoadOp(vk::AttachmentLoadOp::eClear))

        cmdBuf_.beginRenderPass(renderPassBegin, {}); 
        {
            cmdBuf_.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//�󶨹���
            cmdBuf_.draw(3, 1, 0, 0); //����3�����㡢1��ͼԪ����0�����㿪ʼ���ƣ���0��Instance��ʼ
        } 
        cmdBuf_.endRenderPass();
    } 
    cmdBuf_.end();

    //���graphcisQueue��GPU��ʼ����ͼ��
    vk::SubmitInfo submit;
    submit.setCommandBuffers(cmdBuf_) 
        .setWaitSemaphores(imageAvaliable_)
        .setSignalSemaphores(imageDrawFinish_);
    //����graphcisQueue�ύ�����ͬʱָ��һ��fence�������ύ��ɺ��fence��������ź�̬
    Context::GetInstance().get_graphcisQueue().submit(submit, cmdAvaliableFence_);

    //���presentQueue�������ƺõ�ͼ����ʾ����Ļ��
    vk::PresentInfoKHR present;
    present.setImageIndices(imageIndex)//��ʾ��ǰ���ƺõ�ͼ��(imageIndex)
        .setSwapchains(swapchain->get_swapchain())
        .setWaitSemaphores(imageDrawFinish_);//
    if (Context::GetInstance().get_presentQueue().presentKHR(present) != vk::Result::eSuccess) {
        std::cout << "image present failed" << std::endl;
    }

    if (Context::GetInstance().get_device().waitForFences(cmdAvaliableFence_, true, std::numeric_limits<uint64_t>::max()) !=
        vk::Result::eSuccess) {//�ȴ�fence����(cmdAvaliableFence_)����ź�̬(true),�����ó�ʱʱ��Ϊmax
        std::cout << "wait for fence failed" << std::endl;
    }

    Context::GetInstance().get_device().resetFences(cmdAvaliableFence_);
}