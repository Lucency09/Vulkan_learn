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

    const Uniform uniform{ Color{1, 0.5, 0} };

    Renderer::Renderer()
    {
        this->createFence();
        this->createSemaphores();
        this->createCmdBuffers();
        this->createVertexBuffer();
        this->bufferVertexData();
        this->createUniformBuffers();
        this->bufferUniformData();
        this->createDescriptorPool();
        this->allocateSets();
        this->updateSets();
    }

    Renderer::~Renderer()
    {
        auto& device = Context::GetInstance().get_device();
        device.destroyDescriptorPool(this->descriptorPool_);
        this->hostUniformBuffer_.clear();
        this->deviceUniformBuffer_.clear();
        this->hostVertexBuffer_.reset();
        this->deviceVertexBuffer_.reset();

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

    void Renderer::createFence()
    {
        this->cmdAvaliableFence_.resize(maxFlightCount_, nullptr);

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

        for (auto& sem : this->imageDrawFinish_)
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
        this->hostUniformBuffer_.resize(maxFlightCount_);
        this->deviceUniformBuffer_.resize(maxFlightCount_);

        for (auto& buffer : this->hostUniformBuffer_)
        {
            buffer.reset(new Buffer(sizeof(uniform),
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
        }

        for (auto& buffer : this->deviceUniformBuffer_)
        {
            buffer.reset(new Buffer(sizeof(uniform),
                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eDeviceLocal));
        } 
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
        for (int i = 0; i < hostUniformBuffer_.size(); i++) {
            auto& buffer = hostUniformBuffer_[i];
            void* ptr = Context::GetInstance().get_device().mapMemory(buffer->memory, 0, buffer->size);
            memcpy(ptr, &uniform, sizeof(uniform));
            Context::GetInstance().get_device().unmapMemory(buffer->memory);

            copyBuffer(buffer->buffer, deviceUniformBuffer_[i]->buffer, buffer->size, 0, 0);
        }
    }

    void Renderer::createDescriptorPool()
    {
        //Descriptorset描述符集
        vk::DescriptorPoolCreateInfo createInfo;
        vk::DescriptorPoolSize poolSize;
        poolSize.setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(this->maxFlightCount_);
        createInfo.setMaxSets(this->maxFlightCount_)
            .setPoolSizes(poolSize);
        this->descriptorPool_ = Context::GetInstance().get_device().createDescriptorPool(createInfo);
    }

    void Renderer::allocateSets()
    {
        std::vector<vk::DescriptorSetLayout> layouts(this->maxFlightCount_, Context::GetInstance().get_render_process().get_setLayout());
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorPool(this->descriptorPool_)
            .setDescriptorSetCount(this->maxFlightCount_)
            .setSetLayouts(layouts);

        this->sets_ = Context::GetInstance().get_device().allocateDescriptorSets(allocInfo);
        //sets_ 是从this->descriptorPool_中创建的，this->descriptorPool_销毁时也跟着被销毁了，不需要手动回收
    }

    void Renderer::updateSets()
    {
        for (int i = 0; i < sets_.size(); i++) {
            auto& set = sets_[i];
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(this->deviceUniformBuffer_[i]->buffer)
                .setOffset(0)
                .setRange(this->deviceUniformBuffer_[i]->size);

            vk::WriteDescriptorSet writer;
            writer.setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setBufferInfo(bufferInfo)
                .setDstBinding(0)
                .setDstSet(set)
                .setDstArrayElement(0)
                .setDescriptorCount(1);
            Context::GetInstance().get_device().updateDescriptorSets(writer, {});
        }
    }

    void Renderer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset)
    {
        auto cmdBuf = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();

        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(size)
                .setSrcOffset(srcOffset)
                .setDstOffset(dstOffset);
            cmdBuf.copyBuffer(src, dst, region);
        } cmdBuf.end();

        vk::SubmitInfo submit;
        submit.setCommandBuffers(cmdBuf);
        Context::GetInstance().get_graphcisQueue().submit(submit);

        Context::GetInstance().get_device().waitIdle();

        Context::GetInstance().get_commandManager()->FreeCmd(cmdBuf);
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
            , this->imageAvaliable_[curFrame_], nullptr);//设置为无限等待
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

            area.setOffset({})
                .setExtent(swapchain->get_info().imageExtent);//设置屏幕大小，从swapchain中获取

            renderPassBegin.setRenderPass(renderProcess.get_renderPass())//设置渲染流程，从Context中拿
                .setRenderArea(area)//设置在整个屏幕上绘制
                .setFramebuffer(swapchain->get_framebuffers()[imageIndex])//设置在哪个framebuffer上绘制，从swapchain中获取 
                .setClearValues(clearValue);//设置用什么颜色来清除framebuffer(因为在render_process中setLoadOp(vk::AttachmentLoadOp::eClear))

            this->cmdBuf_[curFrame_].beginRenderPass(&renderPassBegin, vk::SubpassContents::eInline);
            {
                this->cmdBuf_[curFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//绑定管线
                vk::DeviceSize offset = 0;
                //this->sets_[curFrame_];
                //this->updateSets();
                this->cmdBuf_[curFrame_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Context::GetInstance().get_render_process().get_layout(), 0, this->sets_[curFrame_], {});
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