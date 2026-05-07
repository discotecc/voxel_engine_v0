cmake_minimum_required(VERSION 3.20)
project(VEngine0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# homebrew prefix hints for compatibility with apple machines...
if (EXISTS "/opt/homebrew")
  list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()
if (EXISTS "/usr/local")
  list(APPEND CMAKE_PREFIX_PATH "/usr/local")
endif()

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(vengine0
  src/main.cpp
  external/glad/src/glad.c      # have to force glad.c to compile into the exe
)

target_include_directories(vengine0 PRIVATE
  external/glad/include
  external/spdlog/include
  external/stbi/include
)

target_link_libraries(vengine0 PRIVATE
  glfw
  glm::glm
  OpenGL::GL
)

if (APPLE)
  target_compile_definitions(vengine0 PRIVATE GL_SILENCE_DEPRECATION)
  target_compile_options(vengine0 PRIVATE -Wall -Wextra -Wpedantic)
endif()
