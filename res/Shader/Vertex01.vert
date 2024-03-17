#version 450
layout (location = 0) in vec3 inPosition;//Ä¬ÈÏbinding=0
layout (location = 1) in vec3 inColor;//Ä¬ÈÏbinding=0

layout(push_constant) uniform Transform {
    mat4 transform;
} transform;

layout(binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(location = 0) out vec4 moutColor;

void main() 
{
    //gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position = transform.transform * vec4(inPosition, 1.0);
    //moutColor = vec4(inPosition, 1.0);
    moutColor = vec4(inColor, 1.0);
}
