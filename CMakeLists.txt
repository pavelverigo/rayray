cmake_minimum_required(VERSION 3.21)

project(rayray)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

add_executable(ppm src/ppm.cpp src/renderer.cpp)

# Main
add_executable(${PROJECT_NAME} src/main.cpp src/renderer.cpp)

# SDL2
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.28.3
)
FetchContent_MakeAvailable(SDL2)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    VERBATIM
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

# OpenGL
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

# GLEW
FetchContent_Declare(
  glew
  URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
)
FetchContent_MakeAvailable(glew)

target_include_directories(${PROJECT_NAME} PRIVATE ${glew_SOURCE_DIR}/include)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${glew_SOURCE_DIR}/bin/Release/x64/glew32.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
  VERBATIM
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${glew_SOURCE_DIR}/lib/Release/x64/glew32.lib")

# IMGUI
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.89.9
)

FetchContent_GetProperties(imgui)
FetchContent_Populate(imgui)
add_library(imgui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} $<TARGET_PROPERTY:SDL2::SDL2,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui)
