#include "Render/Vulkan/include/renderer.hpp"
#include "Render/Vulkan/include/context.hpp"

namespace toy2d
{
    Renderer::Renderer()
    {
        this->init();
    }

    Renderer::Renderer(std::vector<Vertex> vex, std::vector<std::uint32_t> ind)
        : vertices(std::move(vex)), indices(std::move(ind))
    {   
        this->init();
    }

    void Renderer::init()
    {
		this->createFence();
		this->createSemaphores();
		this->createCmdBuffers();
		this->createBuffer();
		this->createUniformBuffers();

		this->updateBuffer();

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
        this->hostIndicesBuffer_.reset();
        this->deviceIndicesBuffer_.reset();

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
        this->hostVertexBuffer_.reset(new Buffer(sizeof(Vertex) * vertices.size(),//设置为三角形顶点大小
            vk::BufferUsageFlagBits::eTransferSrc,//只用作传输
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));//设置为CPU可见 | CPU本地独占
        
        this->deviceVertexBuffer_.reset(new Buffer(sizeof(Vertex) * vertices.size(),//设置为三角形顶点大小
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,//用作传输和顶点Buffer
            vk::MemoryPropertyFlagBits::eDeviceLocal));//仅GPU可见
        
    }

    void Renderer::createIndicesBuffer()
    {
        this->hostIndicesBuffer_.reset(new Buffer(sizeof(std::uint32_t) * this->indices.size(),
            vk::BufferUsageFlagBits::eTransferSrc, 
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        this->deviceIndicesBuffer_.reset(new Buffer(sizeof(std::uint32_t) * this->indices.size(),
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal));
    }

    void Renderer::createBuffer()
    {
        this->createVertexBuffer();
        this->createIndicesBuffer();
    }

    void Renderer::createUniformBuffers()
    {
        this->hostUniformBuffer_.resize(this->uniformCount_);//uniform变量数
        this->deviceUniformBuffer_.resize(this->uniformCount_);

        for (int i = 0; i < this->hostUniformBuffer_.size(); i++) {
            auto& buffers = this->hostUniformBuffer_[i];
            buffers.resize(this->maxFlightCount_);
            for (auto& buffer : buffers) {
                buffer.reset(new Buffer(this->UniformMemorySize[i],
                    vk::BufferUsageFlagBits::eTransferSrc,
                    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
            }
        }

        for (int i = 0; i < this->deviceUniformBuffer_.size(); i++) {
            auto& buffers = this->deviceUniformBuffer_[i];
            buffers.resize(this->maxFlightCount_);
            for (auto& buffer : buffers) {
                buffer.reset(new Buffer(this->UniformMemorySize[i],
                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eDeviceLocal));
            }
        } 
    }

    void Renderer::updateVertexData()
    {
        //为当前设备设置一次内存映射，其参数为本地顶点缓冲的内存，起始偏移量，大小为本地顶点缓冲的大小
        void* ptr = Context::GetInstance().get_device().mapMemory(this->hostVertexBuffer_->memory, 0, this->hostVertexBuffer_->size);
        //将数据vertices拷贝到内存中(这部分内存已经和hostVertexBuffer_有映射关系了，故该操作就是把vertices中的输入拷入this->hostVertexBuffer_-)
        memcpy(ptr, this->vertices.data(), this->hostVertexBuffer_->size);
        //关闭内存映射
        Context::GetInstance().get_device().unmapMemory(this->hostVertexBuffer_->memory);

        this->copyBuffer(this->hostVertexBuffer_->buffer, this->deviceVertexBuffer_->buffer, this->hostVertexBuffer_->size, 0, 0);

    }

    void Renderer::updateIndicesData()
    {
        void* ptr = Context::GetInstance().get_device().mapMemory(this->hostIndicesBuffer_->memory, 0, this->hostIndicesBuffer_->size);
        
        memcpy(ptr, this->indices.data(), this->hostIndicesBuffer_->size);
        Context::GetInstance().get_device().unmapMemory(this->hostIndicesBuffer_->memory);

        this->copyBuffer(this->hostIndicesBuffer_->buffer, this->deviceIndicesBuffer_->buffer, this->hostIndicesBuffer_->size, 0, 0);
    }

    void Renderer::updateBuffer()
    {
        this->updateVertexData();
        this->updateIndicesData();
    }

    void Renderer::updateUniformData(int uniform_binding)
    {
        for (int i = 0; i < hostUniformBuffer_[uniform_binding].size(); i++) {
            auto& buffer = hostUniformBuffer_[uniform_binding][i];
            void* ptr = Context::GetInstance().get_device().mapMemory(buffer->memory, 0, buffer->size);
            if(uniform_binding == static_cast<int>(UBN::COLOR))
                memcpy(ptr, &this->UniformColor, sizeof(UniformColor));
            else if(uniform_binding == static_cast<int>(UBN::MVP))
                memcpy(ptr, &this->mvp, sizeof(mvp));

            Context::GetInstance().get_device().unmapMemory(buffer->memory);

            copyBuffer(buffer->buffer, deviceUniformBuffer_[uniform_binding][i]->buffer, buffer->size, 0, 0);
        }
    }

    void Renderer::createDescriptorPool()
    {
        //Descriptorset描述符集
        vk::DescriptorPoolCreateInfo createInfo;
        vk::DescriptorPoolSize poolSize;
        poolSize.setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(this->maxFlightCount_);
        std::vector<vk::DescriptorPoolSize> sizes(2, poolSize);
        createInfo.setMaxSets(this->maxFlightCount_ * this->uniformCount_)
            .setPoolSizes(sizes);
        this->descriptorPool_ = Context::GetInstance().get_device().createDescriptorPool(createInfo);
        this->sets_.resize(this->maxFlightCount_);
    }

    void Renderer::allocateSets()
    {
        std::vector<vk::DescriptorSetLayout> layouts(this->maxFlightCount_, Context::GetInstance().get_render_process().get_setLayout());
        //get_setLayout()返回值是一个引用，所以这里不需要使用std::move
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorPool(this->descriptorPool_)
            //.setDescriptorSetCount(this->uniformCount_)
            .setSetLayouts(layouts);

        this->sets_ = Context::GetInstance().get_device().allocateDescriptorSets(allocInfo);
        //sets_ 是从this->descriptorPool_中创建的，this->descriptorPool_销毁时也跟着被销毁了，不需要手动回收
    }

    void Renderer::updateSets()
    {// TODO: set布局待更改
        int i = 0;
        for (auto& set : this->sets_) {
            std::array<vk::DescriptorBufferInfo, 2> bufferInfos;
            //分别绑定Color和MVP的uniformbuffer
            bufferInfos[static_cast<int>(UBN::COLOR)]
                .setBuffer(this->deviceUniformBuffer_[static_cast<int>(UBN::COLOR)][i]->buffer)//此处将set和buffer绑定
                .setOffset(0)
                .setRange(this->deviceUniformBuffer_[static_cast<int>(UBN::COLOR)][i]->size);

            bufferInfos[static_cast<int>(UBN::MVP)]
                .setBuffer(this->deviceUniformBuffer_[static_cast<int>(UBN::MVP)][i]->buffer)
                .setOffset(0)
                .setRange(this->deviceUniformBuffer_[static_cast<int>(UBN::MVP)][i]->size);

            std::array <vk::WriteDescriptorSet, 2> writers;
            writers[static_cast<int>(UBN::COLOR)]
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setBufferInfo(bufferInfos[static_cast<int>(UBN::COLOR)])
                .setDstBinding(static_cast<int>(UBN::COLOR))
                .setDstSet(set)
                .setDstArrayElement(0)
                .setDescriptorCount(1);

            writers[static_cast<int>(UBN::MVP)]
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setBufferInfo(bufferInfos[static_cast<int>(UBN::MVP)])
                .setDstBinding(static_cast<int>(UBN::MVP))
                .setDstSet(set)
                .setDstArrayElement(0)
                .setDescriptorCount(1);

            Context::GetInstance().get_device().updateDescriptorSets(writers, {});
            i++;
        }
    }

    void Renderer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset)
    {
        //创建一个临时的cmdBuffer
        auto cmdBuf = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();
        //将命令添加至命令队列，内容为将hostBuffer内的数据传输到deviceBuffer中
        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(size)
                .setSrcOffset(srcOffset)//源偏移量
                .setDstOffset(dstOffset);//目标偏移量
            cmdBuf.copyBuffer(src, dst, region);
        } cmdBuf.end();

        vk::SubmitInfo submit;
        submit.setCommandBuffers(cmdBuf);
        Context::GetInstance().get_graphcisQueue().submit(submit);

        Context::GetInstance().get_device().waitIdle();

        Context::GetInstance().get_commandManager()->FreeCmd(cmdBuf);
    }

    void Renderer::Draw()
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

        //更新UniformBuffer
        this->updateUniformData(static_cast<int>(UBN::COLOR));
        //this->updateSets(static_cast<int>(UBN::COLOR));
        //TODO: 更新MVP
        this->updateUniformData(static_cast<int>(UBN::MVP));
        //this->updateSets(static_cast<int>(UBN::MVP));
        
        //开始向commandbuffer中填充命令
        this->cmdBuf_[curFrame_].reset();//重置Buffer
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);//设置生命周期为使用一次就重置
        this->cmdBuf_[curFrame_].begin(begininfo);
        {
            vk::RenderPassBeginInfo renderPassBegin;
            vk::Rect2D area;
            vk::ClearValue clearValue;
            clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f});

            area.setOffset({})
                .setExtent(swapchain->get_info().imageExtent);//设置屏幕大小，从swapchain中获取

            renderPassBegin.setRenderPass(renderProcess.get_renderPass())//设置渲染流程，从Context中拿
                .setRenderArea(area)//设置在整个屏幕上绘制
                .setFramebuffer(swapchain->get_framebuffers()[imageIndex])//设置在哪个framebuffer上绘制，从swapchain中获取 
                .setClearValues(clearValue);//设置用什么颜色来清除framebuffer(因为在render_process中setLoadOp(vk::AttachmentLoadOp::eClear))

            this->cmdBuf_[curFrame_].beginRenderPass(&renderPassBegin, vk::SubpassContents::eInline);
            {
                this->cmdBuf_[curFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, 
                                renderProcess.get_pipeline());//绑定管线
                vk::DeviceSize offset = 0;
                
                this->cmdBuf_[curFrame_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, 
                                Context::GetInstance().get_render_process().get_layout(), 
                                0,//这个参数是指定在layout中的第几个set，同时是shader程序中的set=?，这里是0
                                this->sets_[curFrame_],
                                {});
                
                this->cmdBuf_[curFrame_].bindVertexBuffers(0, 
                                deviceVertexBuffer_->buffer, 
                                offset);//将deviceVertexBuffer_的数据传入，第一个参数指代存在多个buffer时使用第几个

                this->cmdBuf_[curFrame_].bindIndexBuffer(this->deviceIndicesBuffer_->buffer, 
                                0, 
                                vk::IndexType::eUint32);

                this->cmdBuf_[curFrame_].pushConstants(Context::GetInstance().get_render_process().get_layout(),
                                vk::ShaderStageFlagBits::eVertex,
                                0,
                                sizeof(UniformTrans),
                                &UniformTrans);//将UniformTrans的数据传入

                //this->cmdBuf_[curFrame_].draw(3, 1, 0, 0); //绘制3个顶点、1个图元，第0个顶点开始绘制，第0个Instance开始
                this->cmdBuf_[curFrame_].drawIndexed(6, 1, 0, 0, 0);//绘制6个顶点，1个实例，第0个顶点开始绘制，第0个Instance开始
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

    void Renderer::set__Vertices(std::vector<Vertex> vex)
    {
		this->vertices = std::move(vex);
        this->updateVertexData();
    }

    void Renderer::set_Index(std::vector<std::uint32_t> ind)
    {
        this->indices = std::move(ind);
        this->updateIndicesData();
    }

    void Renderer::random_rotation()
    {
        this->UniformTrans = getrotate(this->UniformTrans);
    }

    void Renderer::set_UniformMVP(const MVP& mvp)
    {
		this->mvp = mvp;
    }

}
