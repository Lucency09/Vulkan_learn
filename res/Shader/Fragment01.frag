#version 450

layout(location = 0) out vec4 outColor;
layout(binding = 0) uniform UBO {
    vec3 color;
} ubo;

void main() 
{
    //outColor = vec4(0.0, 1.0, 0.0, 1.0);
    outColor = vec4(ubo.color, 1);
}

/*
layout(set = 0, binding = 0) uniform xxx{};

每个vk::DescriptorSetLayoutBinding 对应一个binding

vk::DescriptorSetLayout中有多个binding,自身对应一个set，
vk::DescriptorSetLayoutBinding放在vk::DescriptorSetLayout里

vk::PipelineLayout中有多个set
vk::DescriptorSetLayout放在vk::PipelineLayout里
*/