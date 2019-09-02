#include "Renderer.hpp"
#include <assert.h>
#include <cstdlib>

Renderer::Renderer()
{
    _InitInstance();
}

Renderer::~Renderer()
{
    _DeInitInstance();
}

void Renderer::_InitInstance()
{
    VkInstanceCreateInfo instance_create_info {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    
    auto err = vkCreateInstance( &instance_create_info, nullptr, &_instance);
    
    if( VK_SUCCESS != err)
    {
        assert( 1 && "Vulkan error");
        std::exit(-1);
    }
}

void Renderer::_DeInitInstance()
{
    vkDestroyInstance( _instance, nullptr);
    _instance = nullptr;
}