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
	createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//让每一个CommandBuffer可以单独reset
	this->cmdPool_ = Context::GetInstance().get_device().createCommandPool(createInfo);
}

void toy2d::Renderer::allocCmdBuf()
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(this->cmdPool_)//设置从this->cmdPool_中分配
		.setCommandBufferCount(1)//分配1个Buffer
		.setLevel(vk::CommandBufferLevel::ePrimary);//设置为GPU直接执行

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

	//查询下一个可以被绘制的image
	auto result = device.acquireNextImageKHR(Context::GetInstance().get_swapchain()->get_swapchain()
												, std::numeric_limits<uint64_t>::max());//设置为无限等待
	if (result.result != vk::Result::eSuccess)
	{
		std::cout << "acquire next image failed!" << std::endl;
	}

	auto imageIndex = result.value;

	//开始向commandbuffer中填充命令
	this->cmdBuf_.reset();//重置Buffer
	vk::CommandBufferBeginInfo begin;
	begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);//设置生命周期为使用一次就重置
	this->cmdBuf_.begin(begin);
    cmdBuf_.begin(begin); 
    {
        vk::RenderPassBeginInfo renderPassBegin;
        vk::Rect2D area;
        vk::ClearValue clearValue;
        clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

        area.setOffset({ 0, 0 })
            .setExtent(swapchain->get_info().imageExtent);//设置屏幕大小，从swapchain中获取

        renderPassBegin.setRenderPass(renderProcess.get_renderPass())//设置渲染流程，从Context中拿
            .setRenderArea(area)//设置在整个屏幕上绘制
            .setFramebuffer(swapchain->get_framebuffers()[imageIndex])//设置在哪个framebuffer上绘制，从swapchain中获取 
            .setClearValues(clearValue);//设置用什么颜色来清除framebuffer(因为在render_process中setLoadOp(vk::AttachmentLoadOp::eClear))

        cmdBuf_.beginRenderPass(renderPassBegin, {}); 
        {
            cmdBuf_.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//绑定管线
            cmdBuf_.draw(3, 1, 0, 0); //绘制3个顶点、1个图元，第0个顶点开始绘制，第0个Instance开始
        } 
        cmdBuf_.endRenderPass();
    } 
    cmdBuf_.end();

    //填充graphcisQueue，GPU开始绘制图像
    vk::SubmitInfo submit;
    submit.setCommandBuffers(cmdBuf_) 
        .setWaitSemaphores(imageAvaliable_)
        .setSignalSemaphores(imageDrawFinish_);
    //在向graphcisQueue提交命令的同时指定一个fence对象，在提交完成后该fence对象会变成信号态
    Context::GetInstance().get_graphcisQueue().submit(submit, cmdAvaliableFence_);

    //填充presentQueue，将绘制好的图像显示在屏幕上
    vk::PresentInfoKHR present;
    present.setImageIndices(imageIndex)//显示当前绘制好的图像(imageIndex)
        .setSwapchains(swapchain->get_swapchain())
        .setWaitSemaphores(imageDrawFinish_);//
    if (Context::GetInstance().get_presentQueue().presentKHR(present) != vk::Result::eSuccess) {
        std::cout << "image present failed" << std::endl;
    }

    if (Context::GetInstance().get_device().waitForFences(cmdAvaliableFence_, true, std::numeric_limits<uint64_t>::max()) !=
        vk::Result::eSuccess) {//等待fence对象(cmdAvaliableFence_)变成信号态(true),并设置超时时长为max
        std::cout << "wait for fence failed" << std::endl;
    }

    Context::GetInstance().get_device().resetFences(cmdAvaliableFence_);
}