# Copyright (c) 2015-2019 LunarG, Inc.

cmake_minimum_required(VERSION 2.8)
set(MAJOR "1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

get_filename_component(INC_DIR "${CMAKE_SOURCE_DIR}/../x86_64/include" ABSOLUTE)
message("INC_DIR = ${INC_DIR}")
set(VK_INCLUDE_DIRS ${VK_INCLUDE_DIRS} "${INC_DIR}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    get_filename_component(LIB_DIR "${CMAKE_SOURCE_DIR}/../x86_64/lib/debug" ABSOLUTE)
else()
    get_filename_component(LIB_DIR "${CMAKE_SOURCE_DIR}/../x86_64/lib" ABSOLUTE)
endif()
message("LIB_DIR = ${LIB_DIR}")
link_directories(${LIB_DIR})

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(COMMON_COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
    set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -fno-strict-aliasing -fno-builtin-memcmp")

    # For GCC version 7.1 or greater, we need to disable the implicit fallthrough warning since there's no consistent way to satisfy
    # all compilers until they all accept the C++17 standard
    if(CMAKE_COMPILER_IS_GNUCC AND NOT (CMAKE_CXX_COMPILER_VERSION LESS 7.1))
        set(COMMON_COMPILE_FLAGS "${COMMON_COMPILE_FLAGS} -Wimplicit-fallthrough=0")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${COMMON_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_COMPILE_FLAGS} -std=c++11 -fno-rtti")
    if (UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
endif()

get_filename_component(BIN_DIR "${CMAKE_SOURCE_DIR}/../x86_64/bin" ABSOLUTE)
message("BIN_DIR = ${BIN_DIR}")
find_program(GLSLANG_VALIDATOR glslangValidator PATHS ${BIN_DIR})

file(GLOB TEXTURES
  "${PROJECT_SOURCE_DIR}/*.ppm"
  )
file(COPY ${TEXTURES} DESTINATION ${CMAKE_BINARY_DIR})

option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)

if (NOT DEMOS_WSI_SELECTION)
  set(DEMOS_WSI_SELECTION "XCB")
endif()

### VulkanInfo ###

add_executable(vulkaninfo vulkaninfo.c)

if(BUILD_WSI_XCB_SUPPORT)
  find_package(XCB REQUIRED)
  target_include_directories(vulkaninfo PRIVATE ${XCB_INCLUDE_DIRS})
  target_link_libraries(vulkaninfo ${XCB_LIBRARIES})
  target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XCB_KHR)
endif()

if(BUILD_WSI_XLIB_SUPPORT)
  find_package(X11 REQUIRED)
  target_include_directories(vulkaninfo PRIVATE ${X11_INCLUDE_DIR})
  target_link_libraries(vulkaninfo ${X11_LIBRARIES})
  target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_XLIB_KHR)
endif()

if(BUILD_WSI_WAYLAND_SUPPORT)
  find_package(Wayland REQUIRED)
  target_include_directories(vulkaninfo PRIVATE ${WAYLAND_CLIENT_INCLUDE_DIR})
  target_link_libraries(vulkaninfo ${WAYLAND_CLIENT_LIBRARIES})
  target_compile_definitions(vulkaninfo PRIVATE -DVK_USE_PLATFORM_WAYLAND_KHR)
endif()

### vkcube and vkcubepp

if (DEMOS_WSI_SELECTION STREQUAL "XCB")
    if (NOT BUILD_WSI_XCB_SUPPORT)
	message( FATAL_ERROR "Selected XCB for demos build but not building Xcb support" )
    endif()
    find_package(XCB REQUIRED)
    include_directories(${XCB_INCLUDE_DIRS})
    link_libraries(${XCB_LIBRARIES})
    add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
    set(DisplayServer Xcb)
elseif(DEMOS_WSI_SELECTION STREQUAL "XLIB")
    if (NOT BUILD_WSI_XLIB_SUPPORT)
	message( FATAL_ERROR "Selected XLIB for demos build but not building Xlib support" )
    endif()
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    link_libraries(${X11_LIBRARIES})
    add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
    set(DisplayServer Xlib)
elseif(DEMOS_WSI_SELECTION STREQUAL "WAYLAND")
    if (NOT BUILD_WSI_WAYLAND_SUPPORT)
	message( FATAL_ERROR "Selected Wayland for demos build but not building Wayland support" )
    endif()
    find_package(Wayland REQUIRED)
    include_directories(${WAYLAND_CLIENT_INCLUDE_DIR})
    link_libraries(${WAYLAND_CLIENT_LIBRARIES})
    add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
    set(DisplayServer Wayland)
else()
    message( FATAL_ERROR "Unrecognized value for DEMOS_WSI_SELECTION: ${DEMOS_WSI_SELECTION}" )
endif()

set(LIBRARIES "vulkan")

add_custom_command(
    COMMENT "Compiling cube demo vertex shader"
    OUTPUT cube.vert.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${CMAKE_BINARY_DIR}/cube.vert.inc ${CMAKE_SOURCE_DIR}/cube.vert
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/cube.vert
    DEPENDS ${CMAKE_SOURCE_DIR}/cube.vert ${GLSLANG_VALIDATOR}
)
add_custom_command(
    COMMENT "Compiling cube demo fragment shader"
    OUTPUT cube.frag.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${CMAKE_BINARY_DIR}/cube.frag.inc ${CMAKE_SOURCE_DIR}/cube.frag
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/cube.frag
    DEPENDS ${CMAKE_SOURCE_DIR}/cube.frag ${GLSLANG_VALIDATOR}
)

include_directories (
   ${VK_INCLUDE_DIRS}
   ${VK_INCLUDE_DIRS}/vulkan
   "${PROJECT_SOURCE_DIR}"
   ${CMAKE_CURRENT_BINARY_DIR}
   )

if(WIN32)
   link_libraries(${XCB_LIBRARIES} vulkan-${MAJOR} png m)
elseif(UNIX)
   link_libraries(vulkan m)
else()
endif()

target_link_libraries(vulkaninfo ${LIBRARIES})

add_executable(vkcube cube.c ${CMAKE_SOURCE_DIR}/cube.vert ${CMAKE_SOURCE_DIR}/cube.frag cube.vert.inc cube.frag.inc)
target_link_libraries(vkcube ${LIBRARIES})

add_executable(vkcubepp cube.cpp ${CMAKE_SOURCE_DIR}/cube.vert ${CMAKE_SOURCE_DIR}/cube.frag cube.vert.inc cube.frag.inc)
target_link_libraries(vkcubepp ${LIBRARIES})
