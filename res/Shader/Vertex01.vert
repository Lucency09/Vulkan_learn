#version 450
layout (location = 0) in vec3 inPosition;//默认binding=0
layout (location = 1) in vec3 inColor;//默认binding=0
layout (location = 2) in vec3 inNormal;//默认binding=0
layout (location = 3) in vec2 inTexCoord;//默认binding=0

layout(push_constant) uniform Transform {
    mat4 transform;
} transform;

layout(binding = 0) uniform MVP {
    mat4 model;//坐标变换
    mat4 view;//观察变换
    mat4 projection;//投影变换
} mvp;

layout (location = 0) out vec4 moutColor;
layout (location = 1) out vec2 outTexCoord;

void main() 
{
    //gl_Position = transform.transform * vec4(inPosition, 1.0);
    gl_Position = mvp.projection * mvp.view * mvp.model * transform.transform * vec4(inPosition, 1.0);
    moutColor = vec4(inColor, 1.0);
    outTexCoord = inTexCoord;
}
