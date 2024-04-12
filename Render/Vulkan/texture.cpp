#include "Render/Vulkan/include/texture.hpp"
#include "Render/Vulkan/include/context.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace toy2d {

    Texture::Texture(std::string_view filename) {
        int w, h, channel;
        stbi_uc* pixels = stbi_load(filename.data(), &w, &h, &channel, STBI_rgb_alpha);//ǿ��ת��Ϊ4ͨ��
        size_t size = w * h * 4;

        if (!pixels) {
            throw std::runtime_error("image load failed");
        }

        std::unique_ptr<Buffer> buffer(new Buffer(size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
        void* ptr = Context::GetInstance().get_device().mapMemory(buffer->memory, 0, buffer->size);//ӳ���ڴ�
        memcpy(ptr, pixels, size);//��������
        Context::GetInstance().get_device().unmapMemory(buffer->memory);//���ӳ��

        createImage(w, h);
        allocMemory();
        Context::GetInstance().get_device().bindImageMemory(image, memory, 0);//���ڴ�,��ӦbindBufferMemory

        transitionImageLayoutFromUndefine2Dst();//��һ��ת�����֣���δ���嵽����Ŀ�꣬ʹ�ܴ���
        transformData2Image(*buffer, w, h);//�����ݴ��䵽ͼ��
        transitionImageLayoutFromDst2Optimal();//�ڶ���ת�����֣��Ӵ���Ŀ�굽��ɫ��ֻ����ʹ����ɫ����ȡ

        createImageView();

        stbi_image_free(pixels);
    }

    Texture::~Texture() {
        auto& device = Context::GetInstance().get_device();
        device.destroyImageView(this->view);
        device.freeMemory(this->memory);
        device.destroyImage(this->image);
    }

    void Texture::createImage(uint32_t w, uint32_t h) {
        vk::ImageCreateInfo createInfo;
        createInfo.setImageType(vk::ImageType::e2D)
            .setArrayLayers(1)
            .setMipLevels(1)
            .setExtent({ w, h, 1 })
            .setFormat(vk::Format::eR8G8B8A8Srgb)
            .setTiling(vk::ImageTiling::eOptimal)//ָ��Ϊ���Ż��ڴ沼��
            .setInitialLayout(vk::ImageLayout::eUndefined)
            //                   ���ڴ������ݵ�ͼ��           ������ɫ����ȡ(��shader��Sampler�ؼ��ֶ�Ӧ)
            .setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
            .setSamples(vk::SampleCountFlagBits::e1);//������(��������Ϊ�Ա������)
        this->image = Context::GetInstance().get_device().createImage(createInfo);
    }

    void Texture::allocMemory() {
        auto& device = Context::GetInstance().get_device();
        vk::MemoryAllocateInfo allocInfo;
        //��ȡͼ���ڴ�����(������buffer��getBufferMemoryRequirements)
        auto requirements = device.getImageMemoryRequirements(this->image);
        allocInfo.setAllocationSize(requirements.size);

        auto index = Context::GetInstance().QueryBufferMemTypeIndex(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
        allocInfo.setMemoryTypeIndex(index);

        memory = device.allocateMemory(allocInfo);
    }

    void Texture::transformData2Image(Buffer& buffer, uint32_t w, uint32_t h) {
        Context::GetInstance().get_commandManager()->ExecuteCmd(Context::GetInstance().get_graphcisQueue(),
            [&](vk::CommandBuffer cmdBuf) {
                vk::BufferImageCopy region;
                vk::ImageSubresourceLayers subsource;
                subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setMipLevel(0)
                    .setLayerCount(1);
                region.setBufferImageHeight(0)//ָ��ͼ��ĸ߶ȣ����ڴ���������⣬����������RGBA�����Բ���Ҫ����
                    .setBufferOffset(0)
                    .setImageOffset(0)
                    .setImageExtent({ w, h, 1 })
                    .setBufferRowLength(0)//ָ�����������г��ȣ����ڴ���������⣬����������RGBA�����Բ���Ҫ����
                    .setImageSubresource(subsource);
                cmdBuf.copyBufferToImage(buffer.buffer, image,
                    vk::ImageLayout::eTransferDstOptimal,
                    region);
            });
    }

    void Texture::transitionImageLayoutFromUndefine2Dst() {
        Context::GetInstance().get_commandManager()->ExecuteCmd(Context::GetInstance().get_graphcisQueue(),
            [&](vk::CommandBuffer cmdBuf) {
                vk::ImageMemoryBarrier barrier;
                vk::ImageSubresourceRange range;
                range.setLayerCount(1)
                    .setBaseArrayLayer(0)
                    .setLevelCount(1)
                    .setBaseMipLevel(0)
                    .setAspectMask(vk::ImageAspectFlagBits::eColor);
                barrier.setImage(this->image)//��������barrier
                    .setOldLayout(vk::ImageLayout::eUndefined)//δת��ǰΪδ���岼��
                    .setNewLayout(vk::ImageLayout::eTransferDstOptimal)//ת����Ϊ����Ŀ�겼��
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)//ֻ��һ��������У����Բ���Ҫָ������
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setDstAccessMask((vk::AccessFlagBits::eTransferWrite))//ת����ʱ�����д���Ȩ��
                    .setSubresourceRange(range);
                cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
                    {}, {}, nullptr, barrier);//ִ�����ϣ�eTopOfPipe��ʾ���ߵ��ʼ��eTransfer��ʾ����׶Σ��������������׶�֮��
            });
    }

    void Texture::transitionImageLayoutFromDst2Optimal() {
        Context::GetInstance().get_commandManager()->ExecuteCmd(Context::GetInstance().get_graphcisQueue(),
            [&](vk::CommandBuffer cmdBuf) {
                vk::ImageMemoryBarrier barrier;
                vk::ImageSubresourceRange range;
                range.setLayerCount(1)
                    .setBaseArrayLayer(0)
                    .setLevelCount(1)
                    .setBaseMipLevel(0)
                    .setAspectMask(vk::ImageAspectFlagBits::eColor);
                barrier.setImage(this->image)
                    .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setSrcAccessMask((vk::AccessFlagBits::eTransferWrite))
                    .setDstAccessMask((vk::AccessFlagBits::eShaderRead))
                    .setSubresourceRange(range);
                cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                    {}, {}, nullptr, barrier);
            });
    }

    void Texture::createImageView() {
        vk::ImageViewCreateInfo createInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setLevelCount(1)
            .setBaseMipLevel(0);
        createInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setComponents(mapping)
            .setFormat(vk::Format::eR8G8B8A8Srgb)
            .setSubresourceRange(range);
        view = Context::GetInstance().get_device().createImageView(createInfo);
    }

}