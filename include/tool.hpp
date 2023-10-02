#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include "vulkan/vulkan.hpp" 

typedef std::function<vk::SurfaceKHR(vk::Instance)> CreateSurfaceFunc;
namespace toy2d 
{

    template <typename T, typename U>
    void RemoveNosupportedElems
        (std::vector<T>& elems, const std::vector<U>& supportedElems,
        std::function<bool(const T&, const U&)> eq) 
    {
        int i = 0;
        while (i < elems.size()) 
        {
            if (std::find_if(supportedElems.begin(), supportedElems.end(),
                [&](const U& e) {return eq(elems[i], e);}
                )== supportedElems.end()) 
            {
                elems.erase(elems.begin() + i);
            }
            else 
            {
                i++;
            }
        }
    }

}