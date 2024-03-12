#include "math.hpp"

namespace toy2d {

    std::vector<vk::VertexInputAttributeDescription> Vertex::GetAttributeDescription() {
        std::vector<vk::VertexInputAttributeDescription> descriptions;
        descriptions.resize(2);
        // Position attribute
        descriptions[0].setBinding(0)
            .setFormat(vk::Format::eR32G32B32Sfloat)//每个片元3个元素(x,y,z)
            .setLocation(0)//设置layout(location = 0)
            .setOffset(offsetof(Vertex, position));//偏移量

        // Color attribute
        descriptions[1].setBinding(0)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setLocation(1)
            .setOffset(offsetof(Vertex, color));
        return descriptions;
    }

    vk::VertexInputBindingDescription Vertex::GetBindingDescription() {
        vk::VertexInputBindingDescription description;
        description.setBinding(0)
            .setStride(sizeof(Vertex))//传输长度设置为Vertex的长度
            .setInputRate(vk::VertexInputRate::eVertex);//每次以片元为单位传输数据

        return description;
    }

}