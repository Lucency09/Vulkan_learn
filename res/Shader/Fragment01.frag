#version 450

layout(location = 0) in vec4 moutColor;

layout(binding = 1) uniform Color {
    vec3 color;
} color;

layout(location = 0) out vec4 outColor;

void main() 
{
    //outColor = vec4(color.color, 1);
    outColor = moutColor;
}

/*
layout(set = 0, binding = 0) uniform xxx{};

每个vk::DescriptorSetLayoutBinding 对应一个binding

vk::DescriptorSetLayout中有多个binding,自身对应一个set，
vk::DescriptorSetLayoutBinding放在vk::DescriptorSetLayout里

vk::PipelineLayout中有多个set
vk::DescriptorSetLayout放在vk::PipelineLayout里
*/