# Inspired by this project https://github.com/andystanton/glfw-skeleton


cmake_minimum_required(VERSION 3.16)
project(RunAllTests)

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
        RunAllTests.cpp
        ../src/include/ArgumentsParser/ArgumentsParser.cpp
        ../src/include/Shader/abstract/Shader.cpp
        ../src/include/Shader/ComputeShader/ComputeShader.cpp
        ../src/include/Shader/VertexFragmentShader/VertexFragmentShader.cpp
        ../src/include/Particle/Particle.cpp
        ../src/include/ParticleSystem/ParticleSystem.cpp
        ../src/include/ParticleSystemInitializer/ParticleSystemCubeInitializer/ParticleSystemCubeInitializer.cpp
        ../src/include/ParticleSolver/ParticleSolverGPU/ParticleSolverGPU.cpp
        ../src/include/ParticleSolver/ParticleSolverCPUParallel/ParticleSolverCPUParallel.cpp
        ../src/include/ParticleSolver/ParticleSolverCPUSequential/ParticleSolverCPUSequential.cpp
        ../src/include/ParticleSystemInitializer/ParticleSystemLagrange/ParticleSystemLagrange.cpp
        ../src/include/ParticleSolver/ParticleSolverCPUGrid/ParticleSolverCPUGrid.cpp
        ../src/include/Grid/GridCPU.cpp
        ../src/include/Bucket/Bucket.cpp
        tests/abstract/CPUTest/CPUTest.cpp
        tests/abstract/GPUTest/GPUTest.cpp
        tests/EnergyConservationTest/CPUEnergyConservationTest.cpp
        tests/DeterministicSolverTest/DeterministicSolverTest.cpp
        tests/EnergyConservationTest/GPUEnergyConservationTest.cpp
        tests/EnergyConservationTest/EnergyCalculator/EnergyCalculator.cpp
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
        ../src/include/Shader/abstract/
        ../src/include/Shader/ComputeShader/
        ../src/include/Shader/VertexFragmentShader
        ../src/include/Particle/
        ../src/include/ParticleSolver/interface/
        ../src/include/ParticleSolver/ParticleSolverGPU/
        ../src/include/ParticleSolver/ParticleSolverCPUParallel/
        ../src/include/ParticleSolver/ParticleSolverCPUSequential/
        ../src/include/ParticleSolver/ParticleSolverCPUGrid/
        ../src/include/ParticleSystemInitializer/interface/
        ../src/include/ParticleSystemInitializer/ParticleSystemCubeInitializer/
        ../src/include/ParticleSystemInitializer/ParticleSystemLagrange/
        ../src/include/ParticleSystem/
        ../src/include/ArgumentsParser/
        ../src/include/enums/
        ../src/include/Grid/
        ../src/include/Bucket/
        tests/interface/
        tests/abstract/CPUTest/
        tests/abstract/GPUTest/
        tests/EnergyConservationTest/
        tests/EnergyConservationTest/
        tests/EnergyConservationTest/EnergyCalculator/
        tests/DeterministicSolverTest/

)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm OpenMP::OpenMP_CXX)