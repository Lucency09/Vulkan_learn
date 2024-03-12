#include "math.hpp"

namespace toy2d {

    std::vector<vk::VertexInputAttributeDescription> Vertex::GetAttributeDescription() {
        std::vector<vk::VertexInputAttributeDescription> descriptions;
        descriptions.resize(2);
        // Position attribute
        descriptions[0].setBinding(0)
            .setFormat(vk::Format::eR32G32B32Sfloat)//ÿ��ƬԪ3��Ԫ��(x,y,z)
            .setLocation(0)//����layout(location = 0)
            .setOffset(offsetof(Vertex, position));//ƫ����

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
            .setStride(sizeof(Vertex))//���䳤������ΪVertex�ĳ���
            .setInputRate(vk::VertexInputRate::eVertex);//ÿ����ƬԪΪ��λ��������

        return description;
    }

}