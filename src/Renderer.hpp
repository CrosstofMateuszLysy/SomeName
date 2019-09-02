
/* 
 * File:   Renderer.hpp
 * Author: krzysiek
 *
 * Created on 2 września 2019, 16:43
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vulkan/vulkan.hpp>

class Renderer
{
public:
    Renderer();
    ~Renderer();

private:
    
    void _InitInstance();
    void _DeInitInstance();
    
    void _InitDevice();
    void _DeInitDevice();
    
    
    VkInstance  _instance = nullptr;
};



#endif /* RENDERER_HPP */

