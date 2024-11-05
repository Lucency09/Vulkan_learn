#include "Render/Vulkan/include/cumpute.hpp"
#include "Render/Vulkan/include/context.hpp"

toy2d::Cumpute::Cumpute()
{
	//this->instance = Context::get_instance();
	this->shadersource = toy2d::Read_spv_File("res/Spir-v/dxt_encode.spv");
	this->device = Context::GetInstance().get_device();
	this->queue = this->device.getQueue(0, 0);
	this->shadermodule = this->createShaderModule(this->device, this->shadersource);
	this->descriptorSetLayout = this->createdescriptorSetLayout();
	this->piplinelayout = this->createpiplinelayout();
	this->pipeline = this->createCumputePipline();
}

toy2d::Cumpute::Cumpute(const std::string& shaderpath)
{
	this->shadersource = toy2d::Read_spv_File(shaderpath);
	this->device = Context::GetInstance().get_device();
	this->queue = this->device.getQueue(0, 0);
	this->shadermodule = this->createShaderModule(this->device, this->shadersource);
	this->descriptorSetLayout = this->createdescriptorSetLayout();
	this->piplinelayout = this->createpiplinelayout();
	this->pipeline = this->createCumputePipline();
    this->commandPool = this->createCommandPool();
}

void toy2d::Cumpute::run_comput(const Buffer& inputbuffer, const Buffer& outputbuffer)
{
    // 创建图像
    vk::Extent3D extent(WINDOWS_WIDTH, WINDOWS_HIGHT, 1);
    vk::Image inputImage = createImage(this->device, extent, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst);
    vk::Image outputImage = createImage(this->device, extent, vk::Format::eR8G8B8A8Uint, vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferDst);

    // 创建图像视图
    vk::ImageView inputImageView = createImageView(this->device, inputImage, vk::Format::eR8G8B8A8Unorm);
    vk::ImageView outputImageView = createImageView(this->device, outputImage, vk::Format::eR8G8B8A8Uint);

    // 将缓冲区数据复制到图像
    copyBufferToImage(this->device, this->commandPool, this->queue, inputbuffer.buffer, inputImage, WINDOWS_WIDTH, WINDOWS_HIGHT);
    copyBufferToImage(this->device, this->commandPool, this->queue, outputbuffer.buffer, outputImage, WINDOWS_WIDTH, WINDOWS_HIGHT);

    // 创建描述符池
    std::array<vk::DescriptorPoolSize, 1> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 2)
    };
    vk::DescriptorPoolCreateInfo poolInfo({}, 1, poolSizes.size(), poolSizes.data());
    vk::DescriptorPool descriptorPool = this->device.createDescriptorPool(poolInfo);

    // 分配描述符集
    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &this->descriptorSetLayout);
    vk::DescriptorSet descriptorSet = this->device.allocateDescriptorSets(allocInfo).front();

    // 更新描述符集
    vk::DescriptorImageInfo inputImageInfo({}, inputImageView, vk::ImageLayout::eGeneral);
    vk::DescriptorImageInfo outputImageInfo({}, outputImageView, vk::ImageLayout::eGeneral);

    std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {
        vk::WriteDescriptorSet(
            descriptorSet, // dstSet
            0, // dstBinding
            0, // dstArrayElement
            1, // descriptorCount
            vk::DescriptorType::eStorageImage, // descriptorType
            &inputImageInfo, // pImageInfo
            nullptr, // pBufferInfo
            nullptr // pTexelBufferView
        ),
        vk::WriteDescriptorSet(
            descriptorSet, // dstSet
            1, // dstBinding
            0, // dstArrayElement
            1, // descriptorCount
            vk::DescriptorType::eStorageImage, // descriptorType
            &outputImageInfo, // pImageInfo
            nullptr, // pBufferInfo
            nullptr // pTexelBufferView
        )
    };

    this->device.updateDescriptorSets(descriptorWrites, {});

    // 记录命令缓冲区
    vk::CommandBufferAllocateInfo cmdBufferAllocInfo(this->commandPool, vk::CommandBufferLevel::ePrimary, 1);
    vk::CommandBuffer commandBuffer = this->device.allocateCommandBuffers(cmdBufferAllocInfo).front();

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, this->pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, this->piplinelayout, 0, descriptorSet, nullptr);

    // 调用计算着色器
    commandBuffer.dispatch((uint32_t)ceil(WINDOWS_WIDTH / float(16)), (uint32_t)ceil(WINDOWS_HIGHT / float(16)), 1);

    commandBuffer.end();

    // 提交命令缓冲区
    vk::SubmitInfo submitInfo({}, {}, commandBuffer, {});
    this->queue.submit(submitInfo, nullptr);
    this->queue.waitIdle();

    // 清理资源
    this->device.freeCommandBuffers(this->commandPool, commandBuffer);
    this->device.destroyDescriptorPool(descriptorPool);
    this->device.destroyImageView(inputImageView);
    this->device.destroyImageView(outputImageView);
    this->device.destroyImage(inputImage);
    this->device.destroyImage(outputImage);
}

vk::ShaderModule toy2d::Cumpute::createShaderModule(vk::Device device, const std::string& shadersource)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shadersource.size();
	createInfo.pCode = (uint32_t*)shadersource.data();

	vk::ShaderModule shaderModuel = device.createShaderModule(createInfo);
	return shaderModuel;
}

vk::DescriptorSetLayout toy2d::Cumpute::createdescriptorSetLayout()
{
	std::array<vk::DescriptorSetLayoutBinding, 2> layoutBindings = {
		vk::DescriptorSetLayoutBinding(
			0, // binding
			vk::DescriptorType::eStorageImage, // descriptor type
			1, // descriptor count
			vk::ShaderStageFlagBits::eCompute // stage flags
		),
		vk::DescriptorSetLayoutBinding(
			1, // binding
			vk::DescriptorType::eStorageImage, // descriptor type
			1, // descriptor count
			vk::ShaderStageFlagBits::eCompute // stage flags
		)
	};
	vk::DescriptorSetLayoutCreateInfo createinfo({}, layoutBindings);
	return this->device.createDescriptorSetLayout(createinfo);
}


vk::PipelineLayout toy2d::Cumpute::createpiplinelayout()
{
	vk::PipelineLayoutCreateInfo piplineLayoutinfo({}, this->descriptorSetLayout);
	return this->device.createPipelineLayout(piplineLayoutinfo);
}

vk::Pipeline toy2d::Cumpute::createCumputePipline()
{
	vk::PipelineShaderStageCreateInfo shaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, this->shadermodule, "main");
	vk::ComputePipelineCreateInfo piplineCreateInfo({}, shaderStageCreateInfo, this->piplinelayout);
	return this->device.createComputePipeline({},piplineCreateInfo).value;
}

vk::CommandPool toy2d::Cumpute::createCommandPool()
{
    vk::CommandPoolCreateInfo poolInfo({}, 0); // 0 是队列族索引，你需要根据实际情况设置
    return this->device.createCommandPool(poolInfo);
}