# Inspired by this project https://github.com/andystanton/glfw-skeleton


cmake_minimum_required(VERSION 3.16)
project(N-body)

include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3 -march=native -ffast-math")


#
# N-body source code
#
file(GLOB PROJECT_SOURCES_CPP "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")

# Include OpenMP
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND)
    message("OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  endif()


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES_CPP})

# Add specific flags based on Compiler and Build Type
if(MSVC)
  message(STATUS "MSVC Compiler Detected - Adding MSVC specific flags")
  target_compile_options(${PROJECT_NAME} PRIVATE
      # Optimization: /O2 is generally equivalent to -O3 for Release builds
      $<$<CONFIG:Release>:/O2>
      # Fast Math: /fp:fast
      $<$<CONFIG:Release>:/fp:fast>
      # OpenMP: /openmp (ensure VS Installer included OpenMP support)
      /openmp:llvm
      # Add other MSVC flags as needed (e.g., /W4 for warnings)
      /W4
      /EHsc # Standard exception handling
  )
else() # Assuming GCC or Clang
  message(STATUS "GCC/Clang Compiler Detected - Adding GCC/Clang specific flags")
  target_compile_options(${PROJECT_NAME} PRIVATE
      # Optimization: -O3 for Release builds
      $<$<CONFIG:Release>:-O3>
      # Architecture Specific (use if you know your target machine, otherwise omit for portability)
      $<$<CONFIG:Release>:-march=native>
      # Fast Math: -ffast-math
      $<$<CONFIG:Release>:-ffast-math>
      # OpenMP: Use the flag found by FindOpenMP
      ${OpenMP_CXX_FLAGS}
      # Add other GCC/Clang flags as needed (e.g., -Wall for warnings)
      -Wall
      -Wextra
      -pedantic
  )
endif()

add_dependencies(${PROJECT_NAME} glfw glad glm)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad glm OpenMP::OpenMP_CXX)