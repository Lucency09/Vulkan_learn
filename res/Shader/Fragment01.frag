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

ÿ��vk::DescriptorSetLayoutBinding ��Ӧһ��binding

vk::DescriptorSetLayout���ж��binding,�����Ӧһ��set��
vk::DescriptorSetLayoutBinding����vk::DescriptorSetLayout��

vk::PipelineLayout���ж��set
vk::DescriptorSetLayout����vk::PipelineLayout��
*/