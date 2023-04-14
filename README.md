
# N-body simulation


## Table of contents
[Controls](#controls)  
[How to run the project](#how-to-run-the-project)

## Controls
| Key/Input      | Action                     |
|----------------|----------------------------|
| Esc            | Close the program          |
| Space          | Pause/start the simulation |
| Mouse scroll   | Zoom in/out                |
| Click and drag | Move the camera            |
| B              | Activate/Deactivate bloom  |
| I              | Increase bloom intensity   |
| D              | Decrease bloom intensity   |
| Q              | Enable/disable point size  |
  




## How to run the project

In this section you will find what I did to run the project in the next OS.

1. [Ubuntu](#ubuntu)
2. [Arch Linux](#arch-linux)

### Ubuntu
##### 1. Update the system. 
```bash
sudo apt-get update
```
##### 2. Install `cmake`.
```bash
sudo apt-get install cmake
```
##### 3. Install `glm`.
```bash
sudo apt-get install libglm-dev
```
##### 4. Install `glfw`.
```bash
sudo apt-get install libglfw3-dev
```
##### 5. Install `glad`.
```bash
sudo cp -R lib/glad/ /usr/include/
```

##### 6. Run the project.  
Do the next step only once. This command will create a new directory titled `build` and change the location to it.
```bash
mkdir build && cd build
``` 
Build a `Makefile` using CMake.
```bash
cmake ../src/main/
```
Compile the program using the `Makefile`.
```bash
make
```
Now you should be able to run the compiled program.
```bash
./N-body
```

### Arch Linux

##### 1. Update the system.  
```bash
sudo pacman -Syu
```

##### 2. Install `cmake`.  
```bash
sudo pacman -S cmake
```

##### 3. Install `glfw`.  
```bash
sudo pacman -S glfw-x11
```
##### 4. Install `glm`.
```bash
sudo pacman -S glm
```

##### 5. Install `glad`.
```bash
sudo cp -R lib/glad/ /usr/include/
```

##### 6. Run the project.  

Do the next step only once. This command will create a new directory titled `build` and change the location to it.
```bash
mkdir build && cd build
```

Build a `Makefile` using CMake.
```bash
cmake ../src/main/
```
Compile the program using the `Makefile`.
```bash
make
```

Now you should be able to run the compiled program.
```bash
./N-body
```

## Program arguments
- `-v` Configure which version/algorithm you want to use. 
- `-i` Configure which initialization you want (How the bodies are initialized).
- `-n` Configure how many bodies you want to simulate.  


## Available Versions/algorithms
There is a total of `3` versions you can try:
- `-v 1` Particle-particle sequential algorithm (n^2 complexity) using the CPU.
- `-v 2` Particle-particle parallel algorithm (n^2 complexity) using the CPU.
- `-v 3` Particle-particle parallel algorithm (n^2 complexity) using the GPU.


## Available initializations
You can try the next initializations:
- `-i 1` Particles form a cube and have random velocities and masses.
- `-i 2` Particles form a galaxy.
## Benchmark
There's a benchmark (written in Python) available for measuring the performance of each version, which generates different plots for comparison. If you're interested, please read the readme file inside the `benchmark` directory.
