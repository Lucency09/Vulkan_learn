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
    this->sampler = this->createSampler();
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
	this->sampler = this->createSampler();
}


void toy2d::Cumpute::run_comput(const Texture& inputtexture, Buffer& outputbuffer)
{
    // 创建描述符池
    std::array<vk::DescriptorPoolSize, 2> poolSizes = {
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1),
        vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1)
    };
    vk::DescriptorPoolCreateInfo poolInfo({}, 2, poolSizes.size(), poolSizes.data());
    vk::DescriptorPool descriptorPool = this->device.createDescriptorPool(poolInfo);

    // 分配描述符集
    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, 1, &this->descriptorSetLayout);
    vk::DescriptorSet descriptorSet = this->device.allocateDescriptorSets(allocInfo).front();

    // 更新描述符集
    vk::DescriptorImageInfo inputImageInfo(this->sampler, inputtexture.view, vk::ImageLayout::eGeneral);
    vk::DescriptorBufferInfo outputBufferInfo(outputbuffer.buffer, 0, VK_WHOLE_SIZE);

    std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {
        vk::WriteDescriptorSet(
            descriptorSet, // dstSet
            0, // dstBinding
            0, // dstArrayElement
            1, // descriptorCount
            vk::DescriptorType::eCombinedImageSampler, // descriptorType
            &inputImageInfo, // pImageInfo
            nullptr, // pBufferInfo
            nullptr // pTexelBufferView
        ),
        vk::WriteDescriptorSet(
            descriptorSet, // dstSet
            1, // dstBinding
            0, // dstArrayElement
            1, // descriptorCount
            vk::DescriptorType::eStorageBuffer, // descriptorType
            nullptr, // pImageInfo
            &outputBufferInfo, // pBufferInfo
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
    //commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, this->piplinelayout, 0, descriptorSet, nullptr);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, this->piplinelayout, 0, 1, &descriptorSet, 0, nullptr);

    // 调用计算着色器
    commandBuffer.dispatch((uint32_t)ceil(WINDOWS_WIDTH / float(16)), (uint32_t)ceil(WINDOWS_HIGHT / float(16)), 1);

    commandBuffer.end();

    // 提交命令缓冲区
    vk::SubmitInfo submitInfo({}, {}, commandBuffer, {});
    this->queue.submit(submitInfo, nullptr);
    this->queue.waitIdle();


    this->device.freeCommandBuffers(this->commandPool, commandBuffer);
    this->device.destroyDescriptorPool(descriptorPool);

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
            vk::DescriptorType::eCombinedImageSampler, // descriptor type
			1, // descriptor count
			vk::ShaderStageFlagBits::eCompute // stage flags
		),
		vk::DescriptorSetLayoutBinding(
			1, // binding
			vk::DescriptorType::eStorageBuffer, // descriptor type
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

vk::Sampler toy2d::Cumpute::createSampler()
{
    vk::SamplerCreateInfo createInfo;
    createInfo
        //.setMagFilter(vk::Filter::eNearest)//放大时使用最近点采样,适配无符号整形纹理
        //.setMinFilter(vk::Filter::eNearest)//缩小时使用最近点采样
        .setMagFilter(vk::Filter::eLinear)//设置放大后采样方式，这里是就临近点直接取样
        .setMinFilter(vk::Filter::eLinear)//设置缩小后采样方式，这里是就临近点直接取样
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)//设置U轴的寻址模式，如果采样坐标超出纹理区域，这里是重复
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(false)//是否启用各向异性过滤，这里是不启用
        //.setMaxAnisotropy(16)//各向异性过滤的最大值，这里是16
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)//设置边界颜色，这里是不透明黑色，由于是寻址模式为重复，所以不会显示
        .setUnnormalizedCoordinates(false)//是否使用非归一化坐标，这里是不使用
        .setCompareEnable(false)//是否启用颜色比较，这里是不启用
        //.setCompareOp(vk::CompareOp::eAlways)//设置颜色比较的方式，这里是总是通过
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);//设置mipmap采样方式，这里是线性插值
    //.setMipmapMode(vk::SamplerMipmapMode::eNearest); //设置mipmap采样方式,这里是就近
    return Context::GetInstance().get_device().createSampler(createInfo);
   
}
