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

ÿ��vk::DescriptorSetLayoutBinding ��Ӧһ��binding

vk::DescriptorSetLayout���ж��binding,�����Ӧһ��set��
vk::DescriptorSetLayoutBinding����vk::DescriptorSetLayout��

vk::PipelineLayout���ж��set
vk::DescriptorSetLayout����vk::PipelineLayout��
*/