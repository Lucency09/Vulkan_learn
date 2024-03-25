#include "math.hpp"

#include <iostream>

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

    glm::mat4 getrotate(glm::mat4& oldmat4)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        float randomAngleX = glm::radians(dis(gen));
        float randomAngleY = glm::radians(dis(gen));
        float randomAngleZ = glm::radians(dis(gen));
        //std::cout << randomAngleZ << std::endl;

        oldmat4 = glm::rotate(oldmat4, randomAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
        oldmat4 = glm::rotate(oldmat4, randomAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
        oldmat4 = glm::rotate(oldmat4, randomAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));

        return oldmat4;
    }


	glm::mat4 gettranslate(glm::mat4& oldmat4)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-1.0, 1.0);

		float randomX = dis(gen);
		float randomY = dis(gen);
		float randomZ = dis(gen);

		oldmat4 = glm::translate(oldmat4, glm::vec3(randomX, randomY, randomZ));

		return oldmat4;
	}

}