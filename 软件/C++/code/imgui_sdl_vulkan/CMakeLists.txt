cmake_minimum_required(VERSION 3.12)
project(imgui_sdl_vulkan_demo)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE APP_SOURCES "external/imgui/*.cpp")
message(STATUS "APP_STATUS: ${APP_SOURCES}")

add_executable(${PROJECT_NAME} main.cpp ${APP_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui 
  ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backend
)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 Vulkan::Vulkan)