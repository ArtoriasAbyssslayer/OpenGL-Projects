cmake_minimum_required(VERSION 3.15)
project(BlackHoleSimulation VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find OpenGL
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# GLM (header-only math library)
add_subdirectory(external/glm)

# Source files
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/BlackHole.cpp
    src/Renderer.cpp
)

set(HEADERS
    include/Shader.h
    include/Camera.h
    include/BlackHole.h
    include/Renderer.h
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glad
    glm::glm
)

# Copy shaders to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_LINUX)
    target_link_libraries(${PROJECT_NAME} dl pthread)
endif()