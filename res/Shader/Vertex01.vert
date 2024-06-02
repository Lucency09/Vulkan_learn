#version 450
layout (location = 0) in vec3 inPosition;//Ĭ��binding=0
layout (location = 1) in vec3 inColor;//Ĭ��binding=0
layout (location = 2) in vec3 inNormal;//Ĭ��binding=0
layout (location = 3) in vec2 inTexCoord;//Ĭ��binding=0

layout(push_constant) uniform Transform {
    mat4 transform;
} transform;

layout(binding = 0) uniform MVP {
    mat4 model;//����任
    mat4 view;//�۲�任
    mat4 projection;//ͶӰ�任
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
