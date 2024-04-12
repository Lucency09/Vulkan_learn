#pragma once

#include "vulkan/vulkan.hpp"
#include <string_view>

#include "Render/Vulkan/include/buffer.hpp"


#ifndef TEXTURE
#define TEXTURE

namespace toy2d {

    class Texture final {
    public:
        Texture(std::string_view filename);
        ~Texture();

        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;

    private:
        void createImage(uint32_t w, uint32_t h);
        void createImageView();
        void allocMemory();
        uint32_t queryImageMemoryIndex();

        void transitionImageLayoutFromUndefine2Dst();//vk::ImageLayout::eUndefined -> vk::ImageLayout::eTransferDstOptimal
        void transitionImageLayoutFromDst2Optimal();//vk::ImageLayout::eTransferDstOptimal -> vk::ImageLayout::eShaderReadOnlyOptimal
        void transformData2Image(Buffer&, uint32_t w, uint32_t h);
    };


}

#endif // !TEXTURE