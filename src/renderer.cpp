#include "renderer.hpp"
#include "context.hpp"


namespace toy2d
{
    static std::array<Vertex, 3> vertices 
        = {
            Vertex{0.0, -0.5},
            Vertex{0.5, 0.5},
            Vertex{-0.5, 0.5}
        };
    //存储三角形顶点坐标


    Renderer::Renderer()
    {
        //this->initCmdPool();
        //this->allocCmdBuf();
        this->createCmdBuffers();
        this->createSems();

        this->createFence();
        //this->createSemaphores();
        this->createVertexBuffer();
        this->bufferVertexData();
    }

    Renderer::~Renderer()
    {
        this->hostVertexBuffer_.reset();
        this->deviceVertexBuffer_.reset();

        auto& device = Context::GetInstance().get_device();
        //device.freeCommandBuffers(this->cmdPool_, this->cmdBuf_);
        //device.destroyCommandPool(this->cmdPool_);

        for (vk::Semaphore& sem : imageAvaliable_)
        {
            device.destroySemaphore(sem);
        }
        for (vk::Semaphore& sem : imageDrawFinish_)
        {
            device.destroySemaphore(sem);
        }
        for (vk::Fence& fence : cmdAvaliableFence_)
        {
            device.destroyFence(fence);
        }
    }

    void Renderer::initCmdPool()
    {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//让每一个CommandBuffer可以单独reset
        this->cmdPool_ = Context::GetInstance().get_device().createCommandPool(createInfo);
    }

    void Renderer::allocCmdBuf()
    {
        
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(this->cmdPool_)//设置从this->cmdPool_中分配
            .setCommandBufferCount(maxFlightCount_)//分配maxFlightCount_个Buffer
            .setLevel(vk::CommandBufferLevel::ePrimary);//设置为GPU直接执行

        this->cmdBuf_ = Context::GetInstance().get_device().allocateCommandBuffers(allocInfo);
   
    }

    void Renderer::createSems()
    {
        vk::SemaphoreCreateInfo createInfo;

        this->imageAvaliable_.resize(this->maxFlightCount_);
        this->imageDrawFinish_.resize(this->maxFlightCount_);

        for (vk::Semaphore& sem : this->imageAvaliable_)
            sem = Context::GetInstance().get_device().createSemaphore(createInfo);
        for (vk::Semaphore& sem : this->imageDrawFinish_)
            sem = Context::GetInstance().get_device().createSemaphore(createInfo);

    }

    void Renderer::createFence()
    {
        this->cmdAvaliableFence_.resize(maxFlightCount_, nullptr);
        vk::FenceCreateInfo createInfo;

        for (auto& fence : this->cmdAvaliableFence_) {
            vk::FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            fence = Context::GetInstance().get_device().createFence(fenceCreateInfo);
        }
    }

    void Renderer::createSemaphores()
    {
        // 创建一个信号量对象
        auto& device = Context::GetInstance().get_device();
        vk::SemaphoreCreateInfo info;

        this->imageAvaliable_.resize(this->maxFlightCount_);
        this->imageDrawFinish_.resize(this->maxFlightCount_);

        for (auto& sem : this->imageAvaliable_) 
        {
            sem = device.createSemaphore(info);
        }

        for (auto& sem : this->imageAvaliable_) 
        {
            sem = device.createSemaphore(info);
        }
    }

    void Renderer::createCmdBuffers()
    {
        this->cmdBuf_.resize(maxFlightCount_);
        Context::GetInstance().initCommandPool();
        for (auto& cmd : cmdBuf_) 
        {
            cmd = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();
        }
    }

    void Renderer::createVertexBuffer()
    {
        hostVertexBuffer_.reset(new Buffer(sizeof(vertices),//设置为三角形顶点大小
            vk::BufferUsageFlagBits::eTransferSrc,//只用作传输
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));//设置为CPU可见 | CPU本地独占
        
        deviceVertexBuffer_.reset(new Buffer(sizeof(vertices),//设置为三角形顶点大小
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,//用作传输和顶点Buffer
            vk::MemoryPropertyFlagBits::eDeviceLocal));//仅GPU可见
        
    }

    void Renderer::createUniformBuffers()
    {
    }

    void Renderer::bufferVertexData()
    {
        //为当前设备设置一次内存映射，其参数为本地顶点缓冲的内存，第0片区域，大小为本地顶点缓冲的大小
        void* ptr = Context::GetInstance().get_device().mapMemory(this->hostVertexBuffer_->memory, 0, this->hostVertexBuffer_->size);
        //将数据vertices拷贝到内存中
        memcpy(ptr, vertices.data(), sizeof(vertices));
        //关闭内存映射
        Context::GetInstance().get_device().unmapMemory(this->hostVertexBuffer_->memory);

        //创建一个临时的cmdBuffer
        vk::CommandBuffer cmdBuf = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();

        //将命令添加至命令队列，内容为将hostBuffer内的数据传输到deviceBuffer中
        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(this->hostVertexBuffer_->size)
                .setSrcOffset(0)//源偏移量
                .setDstOffset(0);//目标偏移量
            cmdBuf.copyBuffer(this->hostVertexBuffer_->buffer, this->deviceVertexBuffer_->buffer, region);
        } cmdBuf.end();

        vk::SubmitInfo submit;
        submit.setCommandBuffers(cmdBuf);
        Context::GetInstance().get_graphcisQueue().submit(submit);//提交

        Context::GetInstance().get_device().waitIdle();//让GPU等待操作完成

        Context::GetInstance().get_commandManager()->FreeCmd(cmdBuf);
        
    }

    void Renderer::bufferUniformData()
    {
    }

    void Renderer::createDescriptorPool()
    {
    }

    void Renderer::allocateSets()
    {
    }

    void Renderer::updateSets()
    {
    }

    void Renderer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset)
    {
    }

    void Renderer::DrawTriangle()
    {

        vk::Device& device = Context::GetInstance().get_device();
        toy2d::RenderProcess& renderProcess = Context::GetInstance().get_render_process();
        std::unique_ptr<Swapchain>& swapchain = Context::GetInstance().get_swapchain();

        if (device.waitForFences(cmdAvaliableFence_[curFrame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
            throw std::runtime_error("wait for fence failed");
        }
        device.resetFences(cmdAvaliableFence_[curFrame_]);



        //查询下一个可以被绘制的image
        auto result = device.acquireNextImageKHR(Context::GetInstance().get_swapchain()->get_swapchain()
            , std::numeric_limits<uint64_t>::max()
            , this->imageAvaliable_[curFrame_], VK_NULL_HANDLE);//设置为无限等待
        if (result.result != vk::Result::eSuccess)
        {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto imageIndex = result.value;

        //开始向commandbuffer中填充命令
        this->cmdBuf_[curFrame_].reset();//重置Buffer
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);//设置生命周期为使用一次就重置
        this->cmdBuf_[curFrame_].begin(begininfo);
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

            this->cmdBuf_[curFrame_].beginRenderPass(renderPassBegin, {});
            {
                this->cmdBuf_[curFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//绑定管线
                vk::DeviceSize offset = 0;
                this->cmdBuf_[curFrame_].bindVertexBuffers(0, deviceVertexBuffer_->buffer, offset);//将deviceVertexBuffer_的数据传入，第一个参数指代存在多个buffer时使用第几个
                this->cmdBuf_[curFrame_].draw(3, 1, 0, 0); //绘制3个顶点、1个图元，第0个顶点开始绘制，第0个Instance开始
            }
            this->cmdBuf_[curFrame_].endRenderPass();
        }
        this->cmdBuf_[curFrame_].end();

        //填充graphcisQueue，GPU开始绘制图像
        vk::SubmitInfo submit;
        vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submit.setCommandBuffers(cmdBuf_[curFrame_])
            .setWaitSemaphores(imageAvaliable_[curFrame_])
            .setSignalSemaphores(imageDrawFinish_[curFrame_])
            .setWaitDstStageMask(flags);//同步机制待处理
        //在向graphcisQueue提交命令的同时指定一个fence对象，在提交完成后该fence对象会变成信号态
        Context::GetInstance().get_graphcisQueue().submit(submit, cmdAvaliableFence_[curFrame_]);

        //填充presentQueue，将绘制好的图像显示在屏幕上
        vk::PresentInfoKHR present;
        present.setImageIndices(imageIndex)//显示当前绘制好的图像(imageIndex)
            .setSwapchains(swapchain->get_swapchain())
            .setWaitSemaphores(imageDrawFinish_[curFrame_]);//
        if (Context::GetInstance().get_presentQueue().presentKHR(present) != vk::Result::eSuccess) {
            std::cout << "image present failed" << std::endl;
        }

        if (Context::GetInstance().get_device().waitForFences(cmdAvaliableFence_[curFrame_], true, std::numeric_limits<uint64_t>::max()) !=
            vk::Result::eSuccess) {//等待fence对象(cmdAvaliableFence_)变成信号态(true),并设置超时时长为max
            std::cout << "wait for fence failed" << std::endl;
        }

        //Context::GetInstance().get_device().resetFences(cmdAvaliableFence_[curFrame_]);
        curFrame_ = (curFrame_ + 1) % maxFlightCount_;
    }

}