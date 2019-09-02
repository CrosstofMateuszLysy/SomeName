 cmake_minimum_required(VERSION 3.1)

project(SomeName)

set(
	BIN_NAME
	"vkdemo"
)
file(GLOB SOURCES "src/*.cpp")
file(GLOB SOURCES "main.cpp")

include_directories("Vulkan/x86_64/include")
link_directories("Vulkan/x86_64/lib") 
link_directories("Vulkan/x86_64/etc/explicit_layer.d")

find_package(glfw3 3.2 REQUIRED)

set(
	CMAKE_CXX_FLAGS_DEBUG 
	"${CMAKE_CXX_FLAGS_DEBUG} -d -Wall"
)

set (CMAKE_CXX_STANDARD 17)

add_executable(${BIN_NAME} ${SOURCES})

target_link_libraries(${BIN_NAME} glfw)
target_link_libraries(${BIN_NAME} vulkan)