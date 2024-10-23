#version 450

layout (local_size_x = 16, local_size_y = 16) in;

layout (rgba32f, binding = 0) uniform image2D img_output;

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 norm_coords = vec2(pixel_coords) / vec2(imageSize(img_output));

    vec4 color = vec4(norm_coords, 0.0, 1.0);
    imageStore(img_output, pixel_coords, color);
}