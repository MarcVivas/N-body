cmake_minimum_required(VERSION 3.16.3)
project(N-body)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp ../../lib/glad/glad.c)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC GL dl glfw OpenMP::OpenMP_CXX)


