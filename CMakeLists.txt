cmake_minimum_required(VERSION 3.20)

# Project name
project(ray_tracing)

# Set the C standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include the headers
include_directories(include)

# Add the executable
add_executable(${PROJECT_NAME}
  src/main.c
  src/twoD.c
)

# Set output directory
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Adding dependency: SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

# Adding dependency: raylib
find_package(raylib CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} ${raylib_LIBRARIES})
