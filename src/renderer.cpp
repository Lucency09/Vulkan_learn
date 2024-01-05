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
    //�洢�����ζ�������


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
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);//��ÿһ��CommandBuffer���Ե���reset
        this->cmdPool_ = Context::GetInstance().get_device().createCommandPool(createInfo);
    }

    void Renderer::allocCmdBuf()
    {
        
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(this->cmdPool_)//���ô�this->cmdPool_�з���
            .setCommandBufferCount(maxFlightCount_)//����maxFlightCount_��Buffer
            .setLevel(vk::CommandBufferLevel::ePrimary);//����ΪGPUֱ��ִ��

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
        // ����һ���ź�������
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
        hostVertexBuffer_.reset(new Buffer(sizeof(vertices),//����Ϊ�����ζ����С
            vk::BufferUsageFlagBits::eTransferSrc,//ֻ��������
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));//����ΪCPU�ɼ� | CPU���ض�ռ
        
        deviceVertexBuffer_.reset(new Buffer(sizeof(vertices),//����Ϊ�����ζ����С
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,//��������Ͷ���Buffer
            vk::MemoryPropertyFlagBits::eDeviceLocal));//��GPU�ɼ�
        
    }

    void Renderer::createUniformBuffers()
    {
    }

    void Renderer::bufferVertexData()
    {
        //Ϊ��ǰ�豸����һ���ڴ�ӳ�䣬�����Ϊ���ض��㻺����ڴ棬��0Ƭ���򣬴�СΪ���ض��㻺��Ĵ�С
        void* ptr = Context::GetInstance().get_device().mapMemory(this->hostVertexBuffer_->memory, 0, this->hostVertexBuffer_->size);
        //������vertices�������ڴ���
        memcpy(ptr, vertices.data(), sizeof(vertices));
        //�ر��ڴ�ӳ��
        Context::GetInstance().get_device().unmapMemory(this->hostVertexBuffer_->memory);

        //����һ����ʱ��cmdBuffer
        vk::CommandBuffer cmdBuf = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();

        //�����������������У�����Ϊ��hostBuffer�ڵ����ݴ��䵽deviceBuffer��
        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(this->hostVertexBuffer_->size)
                .setSrcOffset(0)//Դƫ����
                .setDstOffset(0);//Ŀ��ƫ����
            cmdBuf.copyBuffer(this->hostVertexBuffer_->buffer, this->deviceVertexBuffer_->buffer, region);
        } cmdBuf.end();

        vk::SubmitInfo submit;
        submit.setCommandBuffers(cmdBuf);
        Context::GetInstance().get_graphcisQueue().submit(submit);//�ύ

        Context::GetInstance().get_device().waitIdle();//��GPU�ȴ��������

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



        //��ѯ��һ�����Ա����Ƶ�image
        auto result = device.acquireNextImageKHR(Context::GetInstance().get_swapchain()->get_swapchain()
            , std::numeric_limits<uint64_t>::max()
            , this->imageAvaliable_[curFrame_], VK_NULL_HANDLE);//����Ϊ���޵ȴ�
        if (result.result != vk::Result::eSuccess)
        {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto imageIndex = result.value;

        //��ʼ��commandbuffer���������
        this->cmdBuf_[curFrame_].reset();//����Buffer
        vk::CommandBufferBeginInfo begininfo;
        begininfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);//������������Ϊʹ��һ�ξ�����
        this->cmdBuf_[curFrame_].begin(begininfo);
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

            this->cmdBuf_[curFrame_].beginRenderPass(renderPassBegin, {});
            {
                this->cmdBuf_[curFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//�󶨹���
                vk::DeviceSize offset = 0;
                this->cmdBuf_[curFrame_].bindVertexBuffers(0, deviceVertexBuffer_->buffer, offset);//��deviceVertexBuffer_�����ݴ��룬��һ������ָ�����ڶ��bufferʱʹ�õڼ���
                this->cmdBuf_[curFrame_].draw(3, 1, 0, 0); //����3�����㡢1��ͼԪ����0�����㿪ʼ���ƣ���0��Instance��ʼ
            }
            this->cmdBuf_[curFrame_].endRenderPass();
        }
        this->cmdBuf_[curFrame_].end();

        //���graphcisQueue��GPU��ʼ����ͼ��
        vk::SubmitInfo submit;
        vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submit.setCommandBuffers(cmdBuf_[curFrame_])
            .setWaitSemaphores(imageAvaliable_[curFrame_])
            .setSignalSemaphores(imageDrawFinish_[curFrame_])
            .setWaitDstStageMask(flags);//ͬ�����ƴ�����
        //����graphcisQueue�ύ�����ͬʱָ��һ��fence�������ύ��ɺ��fence��������ź�̬
        Context::GetInstance().get_graphcisQueue().submit(submit, cmdAvaliableFence_[curFrame_]);

        //���presentQueue�������ƺõ�ͼ����ʾ����Ļ��
        vk::PresentInfoKHR present;
        present.setImageIndices(imageIndex)//��ʾ��ǰ���ƺõ�ͼ��(imageIndex)
            .setSwapchains(swapchain->get_swapchain())
            .setWaitSemaphores(imageDrawFinish_[curFrame_]);//
        if (Context::GetInstance().get_presentQueue().presentKHR(present) != vk::Result::eSuccess) {
            std::cout << "image present failed" << std::endl;
        }

        if (Context::GetInstance().get_device().waitForFences(cmdAvaliableFence_[curFrame_], true, std::numeric_limits<uint64_t>::max()) !=
            vk::Result::eSuccess) {//�ȴ�fence����(cmdAvaliableFence_)����ź�̬(true),�����ó�ʱʱ��Ϊmax
            std::cout << "wait for fence failed" << std::endl;
        }

        //Context::GetInstance().get_device().resetFences(cmdAvaliableFence_[curFrame_]);
        curFrame_ = (curFrame_ + 1) % maxFlightCount_;
    }

}