
# N-body simulation

## Preview

## Table of contents
- [About this project](#about-this-project)  
- [Controls](#controls)  
- [How to run the project](#how-to-run-the-project)  
- [Program arguments](#program-arguments)  
- [Available versions](#available-versions)  
- [Available initializations](#available-initializations)  


## About this project  
This project was made to learn `C++`, `OpenGL` and `GPU programming`.

The program offers: 
- Different versions/algorithms can be tried.
- Different intializations can be tried.
- The number of bodies to simulate can be customized.
- The simulation is in a 3D world.
- Particles have bloom (only the brightest ones).

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

1. [Linux](#linux)

### Linux

#### Previous requirements
You need to have installed:
- `git`
- `xorg-dev`
- `cmake`
- `make`

#### Run the program
This command will create a new directory titled `build` and change the location to it.
```bash
mkdir build && cd build
``` 
Build a `Makefile` using `cmake`.
```bash
cmake ..
```
Compile the program using the `Makefile`.
```bash
make
```
Run the compiled program.
```bash
./N-body
```


## Program arguments
- `-v` Configure which [version](#available-versions) you want to use. 
- `-i` Configure which [initialization](#available-initializations) you want (How the bodies are initialized).
- `-n` Configure how many bodies you want to simulate.  

The default arguments are:
```bash
./N-body -v 1 -i 2 -n 100  
```

## Available versions
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
