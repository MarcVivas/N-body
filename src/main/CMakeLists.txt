cmake_minimum_required(VERSION 3.16.3)
project(N-body)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cpp ../../lib/glad/glad.c)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC GL dl glfw)

