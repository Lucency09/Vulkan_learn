#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <direct.h>

#include "vulkan/vulkan.hpp" 

typedef std::function<vk::SurfaceKHR(vk::Instance)> CreateSurfaceFunc;
namespace toy2d 
{
    template <typename T, typename U>
    inline void RemoveNosupportedElems
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
    


    //��ȡspir-v�ļ�
    inline std::string Read_spv_File(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (!file.is_open()) 
        {
            std::cout << "read " << filename << " failed" << std::endl;
            return std::string{};
        }

        auto size = file.tellg();
        std::string content;
        content.resize(size);
        file.seekg(0);

        file.read(content.data(), content.size());

        return content;
    }
    
    // һ���������������ڻ�ȡһ��Ŀ¼�µ�����ָ����׺�����ļ���
    inline std::vector<std::string> get_files_with_extension(const std::string& dir, const std::string& ext) 
    {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(dir)) 
        {
            if (entry.is_regular_file() && entry.path().extension() == ext) 
            {
                files.push_back(entry.path().filename().string());
            }
        }
        return files;
    }

    // һ�����������������滻һ���ַ����е�ĳ���Ӵ�Ϊ��һ���Ӵ�
    inline std::string replace_substring(const std::string& str, const std::string& from, const std::string& to) 
    {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) 
        {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    // ���ڱ���glsl����Ϊspir-v��ʽ
    inline void compile_glsl(const std::string& file_type, const std::string& glslangValidator_path, const std::string& input_dir, const std::string& output_dir)
    {
        std::string end_str = "";
        std::string par = "";
        if (file_type == "vertext")
        {
            end_str = ".vert";
            par = "-V";
        }
        else if (file_type == "fragment")
        {
            end_str = ".frag";
            par = "-S frag -V";
        }
        
        // ��ȡ����Ŀ¼�µ�����.shader�ļ�
        std::vector<std::string> shader_files = get_files_with_extension(input_dir, end_str);
    
        // ����ÿ���ļ���ִ�б����������������浽���Ŀ¼��
        for (const auto& file : shader_files) 
        {
            // �������������磺
            // glslangValidator -V input_dir/file -o output_dir/file.spv
            std::string command = glslangValidator_path +  " " + par + " " + input_dir + file + " -o " + output_dir + replace_substring(file, end_str, ".spv");
            std::cout << "run cmd: " << command << std::endl;
            // ִ�б����������鷵��ֵ
            int ret = system(command.c_str());
            if (ret != 0) 
            {
                // �������ֵ��Ϊ0��˵�����������ӡ������Ϣ���˳�
                std::cerr << "Error: failed to compile " << file << std::endl;
                exit(1);
            }
            else 
            {
                // �������ֵΪ0��˵������ɹ�����ӡ�ɹ���Ϣ
                std::cout << "Success: compiled " << file << " to " << replace_substring(file, end_str, ".spv") << std::endl;
            }
        }
    }

    //���ڱ���ָ��·���µ�������ɫ���ļ�
    inline void comile_shader(const std::string& input_dir, const std::string& output_dir)
    {
        std::cout << "��ʼ����shader" << std::endl;
        compile_glsl("vertext", "glslangValidator", input_dir, output_dir);
        compile_glsl("fragment", "glslangValidator", input_dir, output_dir);
        std::cout << "��������shader" << std::endl;
    }
}