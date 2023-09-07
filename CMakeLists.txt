cmake_minimum_required(VERSION 3.21)

project(rayray)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_executable(ppm src/ppm.cpp)

# Main
add_executable(${PROJECT_NAME} src/main.cpp)

find_package(SDL2 QUIET)

if (NOT SDL2_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.28.2
  )
  FetchContent_MakeAvailable(SDL2)

  add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
      VERBATIM
  )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)

find_package(glew QUIET)

if (NOT glew_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    glew
    URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
  )

  FetchContent_MakeAvailable(glew)

  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${glew_SOURCE_DIR}/bin/Release/x64/glew32.dll" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    VERBATIM
  )

  target_include_directories(${PROJECT_NAME} PRIVATE ${glew_SOURCE_DIR}/include)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${glew_SOURCE_DIR}/lib/Release/x64/glew32.lib)
endif()


