#include "renderer.hpp"
#include "context.hpp"

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
		this->updateUniformData();

		this->createDescriptorPool();
		this->allocateSets(static_cast<int>(UBN::COLOR));
		this->updateSets(static_cast<int>(UBN::COLOR));
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
        // ����һ���ź�������
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
        this->hostVertexBuffer_.reset(new Buffer(sizeof(Vertex) * vertices.size(),//����Ϊ�����ζ����С
            vk::BufferUsageFlagBits::eTransferSrc,//ֻ��������
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));//����ΪCPU�ɼ� | CPU���ض�ռ
        
        this->deviceVertexBuffer_.reset(new Buffer(sizeof(Vertex) * vertices.size(),//����Ϊ�����ζ����С
            vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,//��������Ͷ���Buffer
            vk::MemoryPropertyFlagBits::eDeviceLocal));//��GPU�ɼ�
        
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
        this->hostUniformBuffer_.resize(this->uniformCount_);//uniform������
        this->deviceUniformBuffer_.resize(this->uniformCount_);

        for (auto& buffers : this->hostUniformBuffer_) {
            buffers.resize(this->maxFlightCount_);
            for (auto& buffer : buffers) {
                buffer.reset(new Buffer(sizeof(UniformColor),//TODO: Uniformbuffer��С������
                    vk::BufferUsageFlagBits::eTransferSrc,
                    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
            }
        }

        for (auto& buffers : this->deviceUniformBuffer_) {
            buffers.resize(this->maxFlightCount_);
            for (auto& buffer : buffers) {
                buffer.reset(new Buffer(sizeof(UniformColor),
                    vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                    vk::MemoryPropertyFlagBits::eDeviceLocal));
            }
        } 
    }

    void Renderer::updateVertexData()
    {
        //Ϊ��ǰ�豸����һ���ڴ�ӳ�䣬�����Ϊ���ض��㻺����ڴ棬��ʼƫ��������СΪ���ض��㻺��Ĵ�С
        void* ptr = Context::GetInstance().get_device().mapMemory(this->hostVertexBuffer_->memory, 0, this->hostVertexBuffer_->size);
        //������vertices�������ڴ���(�ⲿ���ڴ��Ѿ���hostVertexBuffer_��ӳ���ϵ�ˣ��ʸò������ǰ�vertices�е����뿽��this->hostVertexBuffer_-)
        memcpy(ptr, this->vertices.data(), this->hostVertexBuffer_->size);
        //�ر��ڴ�ӳ��
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

    void Renderer::updateUniformData()//TODO: �����ģ����binding��
    {
        for (int i = 0; i < hostUniformBuffer_[static_cast<int>(UBN::COLOR)].size(); i++) {
            auto& buffer = hostUniformBuffer_[static_cast<int>(UBN::COLOR)][i];
            void* ptr = Context::GetInstance().get_device().mapMemory(buffer->memory, 0, buffer->size);
            memcpy(ptr, &UniformColor, sizeof(UniformColor));
            Context::GetInstance().get_device().unmapMemory(buffer->memory);

            copyBuffer(buffer->buffer, deviceUniformBuffer_[static_cast<int>(UBN::COLOR)][i]->buffer, buffer->size, 0, 0);
        }
    }

    void Renderer::createDescriptorPool()
    {
        //Descriptorset��������
        vk::DescriptorPoolCreateInfo createInfo;
        vk::DescriptorPoolSize poolSize;
        poolSize.setType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(this->uniformCount_);
        createInfo.setMaxSets(this->maxFlightCount_ * this->uniformCount_)
            .setPoolSizes(poolSize);
        this->descriptorPool_ = Context::GetInstance().get_device().createDescriptorPool(createInfo);
        this->sets_.resize(this->uniformCount_);
    }

    void Renderer::allocateSets(int binding_num)
    {
        std::vector<vk::DescriptorSetLayout> layouts(this->maxFlightCount_, Context::GetInstance().get_render_process().get_setLayout());
        //get_setLayout()����ֵ��һ�����ã��������ﲻ��Ҫʹ��std::move
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorPool(this->descriptorPool_)
            .setDescriptorSetCount(this->maxFlightCount_)
            .setSetLayouts(layouts);

        this->sets_[binding_num] = Context::GetInstance().get_device().allocateDescriptorSets(allocInfo);
        //sets_ �Ǵ�this->descriptorPool_�д����ģ�this->descriptorPool_����ʱҲ���ű������ˣ�����Ҫ�ֶ�����
    }

    void Renderer::updateSets(int binding_num)
    {
        for (int i = 0; i < sets_[binding_num].size(); i++) {
            auto& set = sets_[binding_num][i];
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(this->deviceUniformBuffer_[static_cast<int>(UBN::COLOR)][i]->buffer)//�˴���set��buffer��
                .setOffset(0)
                .setRange(this->deviceUniformBuffer_[static_cast<int>(UBN::COLOR)][i]->size);

            vk::WriteDescriptorSet writer;
            writer.setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setBufferInfo(bufferInfo)
                .setDstBinding(binding_num)
                .setDstSet(set)
                .setDstArrayElement(0)
                .setDescriptorCount(1);
            Context::GetInstance().get_device().updateDescriptorSets(writer, {});
        }
    }

    void Renderer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset)
    {
        //����һ����ʱ��cmdBuffer
        auto cmdBuf = Context::GetInstance().get_commandManager()->CreateOneCommandBuffer();
        //�����������������У�����Ϊ��hostBuffer�ڵ����ݴ��䵽deviceBuffer��
        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(size)
                .setSrcOffset(srcOffset)//Դƫ����
                .setDstOffset(dstOffset);//Ŀ��ƫ����
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

        //��ѯ��һ�����Ա����Ƶ�image
        auto result = device.acquireNextImageKHR(Context::GetInstance().get_swapchain()->get_swapchain()
            , std::numeric_limits<uint64_t>::max()
            , this->imageAvaliable_[curFrame_], nullptr);//����Ϊ���޵ȴ�
        if (result.result != vk::Result::eSuccess)
        {
            std::cout << "acquire next image failed!" << std::endl;
        }

        auto imageIndex = result.value;

        //����UniformBuffer
        this->updateUniformData();

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

            area.setOffset({})
                .setExtent(swapchain->get_info().imageExtent);//������Ļ��С����swapchain�л�ȡ

            renderPassBegin.setRenderPass(renderProcess.get_renderPass())//������Ⱦ���̣���Context����
                .setRenderArea(area)//������������Ļ�ϻ���
                .setFramebuffer(swapchain->get_framebuffers()[imageIndex])//�������ĸ�framebuffer�ϻ��ƣ���swapchain�л�ȡ 
                .setClearValues(clearValue);//������ʲô��ɫ�����framebuffer(��Ϊ��render_process��setLoadOp(vk::AttachmentLoadOp::eClear))

            this->cmdBuf_[curFrame_].beginRenderPass(&renderPassBegin, vk::SubpassContents::eInline);
            {
                this->cmdBuf_[curFrame_].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess.get_pipeline());//�󶨹���
                vk::DeviceSize offset = 0;
                this->cmdBuf_[curFrame_].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, 
                                Context::GetInstance().get_render_process().get_layout(), 
                                0, 
                                this->sets_[static_cast<int>(UBN::COLOR)][curFrame_], 
                                {});
                this->cmdBuf_[curFrame_].bindVertexBuffers(0, 
                                deviceVertexBuffer_->buffer, 
                                offset);//��deviceVertexBuffer_�����ݴ��룬��һ������ָ�����ڶ��bufferʱʹ�õڼ���
                this->cmdBuf_[curFrame_].bindIndexBuffer(this->deviceIndicesBuffer_->buffer, 
                                0, 
                                vk::IndexType::eUint32);
                this->cmdBuf_[curFrame_].pushConstants(Context::GetInstance().get_render_process().get_layout(),
                                vk::ShaderStageFlagBits::eVertex,
                                0,
                                sizeof(UniformTrans),
                                &UniformTrans);//��UniformTrans�����ݴ���
                //this->cmdBuf_[curFrame_].draw(3, 1, 0, 0); //����3�����㡢1��ͼԪ����0�����㿪ʼ���ƣ���0��Instance��ʼ
                this->cmdBuf_[curFrame_].drawIndexed(6, 1, 0, 0, 0);//����6�����㣬1��ʵ������0�����㿪ʼ���ƣ���0��Instance��ʼ
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

    void Renderer::set__Vertices(std::vector<Vertex> vex)
    {
		this->vertices = std::move(vex);
    }

    void Renderer::set_Index(std::vector<std::uint32_t> ind)
    {
        this->indices = std::move(ind);
    }

}
