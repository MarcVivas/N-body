# Inspired by this project https://github.com/andystanton/glfw-skeleton


cmake_minimum_required(VERSION 3.16)
project(N-body)

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FETCHCONTENT_QUIET FALSE)

#
# glfw
#
set(GLFW_BUILD_DOCS FALSE)
set(GLFW_BUILD_EXAMPLES FALSE)
set(GLFW_BUILD_TESTS FALSE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.4
  GIT_PROGRESS TRUE)

#
# glad
#
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad
  GIT_TAG v0.1.34
  GIT_PROGRESS TRUE)

#
# glm
#
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG 0.9.9.8
  GIT_PROGRESS TRUE)

#
# fetch dependencies
#
FetchContent_MakeAvailable(glfw glad glm)

#
# N-body source code
#
set(
        PROJECT_SOURCES
        src/main/main.cpp
        src/include/ArgumentsParser/ArgumentsParser.cpp
        src/include/Camera/Camera.cpp
        src/include/Shader/abstract/Shader.cpp
        src/include/Shader/ComputeShader/ComputeShader.cpp
        src/include/Shader/VertexFragmentShader/VertexFragmentShader.cpp
        src/include/Particle/Particle.cpp
        src/include/ParticleSystem/ParticleSystem.cpp
        src/include/ParticleSystemInitializer/ParticleSystemCubeInitializer/ParticleSystemCubeInitializer.cpp
        src/include/ParticleSystemInitializer/ParticleSystemGalaxyInitializer/ParticleSystemGalaxyInitializer.cpp
        src/include/ParticleSystemInitializer/ParticleSystemSphere/ParticleSystemSphere.cpp
        src/include/ParticleSystemInitializer/ParticleSystemBall/ParticleSystemBall.cpp
        src/include/ParticleSystemInitializer/ParticleSystemCubeSurface/ParticleSystemCubeSurface.cpp
        src/include/ParticleSystemInitializer/ParticleSystemLagrange/ParticleSystemLagrange.cpp
        src/include/ParticleSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp
        src/include/ParticleSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp
        src/include/ParticleSolver/ParticleSolverGPU/ParticleSolverGPU.cpp
        src/include/Bloom/Bloom.cpp
        src/include/Window/Window.cpp
        src/include/RenderTimer/RenderTimer.cpp
        src/include/ParticleDrawer/ParticleDrawer.cpp
        src/include/ParticleSimulation/ParticleSimulation.cpp
        src/include/RenderLoop/RenderLoop.cpp
        src/include/WindowInputManager/WindowInputManager.cpp
        src/include/Bucket/Bucket.cpp
        src/include/Grid/GridCPU.cpp
        src/include/Grid/GridGPU.cpp
        src/include/ParticleSolver/ParticleSolverCPUGrid/ParticleSolverCPUGrid.cpp
        src/include/ParticleSolver/ParticleSolverGPUGrid/ParticleSolverGPUGrid.cpp



)

# Include OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

add_dependencies(${PROJECT_NAME} glfw glad glm)

target_include_directories(
        ${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/enums/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ArgumentsParser/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Camera/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Shader/abstract/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Shader/ComputeShader/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Shader/VertexFragmentShader/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Particle/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystem/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/interface/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemCubeInitializer/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemGalaxyInitializer/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemSphere/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemBall/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemCubeSurface/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/interface/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/ParticleSolverCPUSequential/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/ParticleSolverCPUParallel/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/ParticleSolverGPU/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/ParticleSolverCPUGrid/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSolver/ParticleSolverGPUGrid/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Bloom/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Bucket/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Window/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/RenderTimer/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleDrawer/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSimulation/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/RenderLoop/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/Grid/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/WindowInputManager/
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ParticleSystemInitializer/ParticleSystemLagrange/
)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm OpenMP::OpenMP_CXX)