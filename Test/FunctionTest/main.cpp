#include "Render/include/Vulkan_Program.h"
#include <iostream>


int main() 
{
    //toy2d::comile_shader("res/Shader/", "res/Spir-v/");
    toy2d::comile_shader("E:\\myproject\\C++project\\Vulkan_learn\\res\\Shader\\",
        "E:\\myproject\\C++project\\Vulkan_learn\\res\\Spir-v\\");
    
    
    Vulkan_program app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    //check_Extension();
    return EXIT_SUCCESS;
}