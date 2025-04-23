# N-body Simulation

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A 3D N-body simulation project developed to explore C++, OpenGL graphics, CPU and GPU parallel programming techniques (OpenMP & Compute Shaders). It simulates particle interactions (e.g., gravitational) using various algorithms from brute-force to Barnes-Hut, allowing for performance comparisons.

![Simulation Showcase](media/overview.gif)
*Demonstration of a simulation run with 37.000 particles*

## Overview
![Overview](media/overview.gif)
![Overview](media/37000.gif)
![Overview](media/system.png)


## Table of contents
- [Controls](#controls)  
- [How to run the project](#how-to-run-the-project)  
- [Program arguments](#program-arguments)  
- [Available versions](#available-versions)  
- [Available initializations](#available-initializations)  
- [Program structure](#program-structure)

### Features
- Different versions/algorithms can be tried.
- Different initializations can be tried.
- 3D simulation.
- Particles have bloom.
- Plummer softening.
- Customizable number of bodies, step size and squared softening.

## Controls
| Key/Input      | Action                                   |
|----------------|------------------------------------------|
| Esc            | Close the program                        |
| Space          | Pause/start the simulation               |
| Mouse scroll   | Zoom in/out                              |
| Click and drag | Move the camera                          |
| B              | Activate/Deactivate bloom                |
| I              | Increase bloom intensity                 |
| D              | Decrease bloom intensity                 |
| Q              | Enable/disable point size                |

## How to run the project

1. [Linux](#linux)
2. [Windows](#windows)

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

### Windows
#### Previous requirements
- `git`
- `cmake`
- `OpenMP`

#### Compile and run the program
These commands will create a new directory titled `build` and change the location to it.
```bash
mkdir build 
cd build
``` 

Generate the build files.
```bash
cmake ..
```

Compile the program.
```bash
cmake --build . --config Release
```

Run the executable.
```bash
.\Release\N-body.exe
```

## Program arguments
- `-v (number)` Configure which [version](#available-versions) you want to use. 
- `-i (number)` Configure which [initialization](#available-initializations) you want (How the bodies are initialized).
- `-n (number)` Configure how many bodies you want to simulate. 
- `-t (decimalNumber)` Configure the step size. (Smaller the more precise)
- `-s (decimalNumber)` Configure the squared softening. 
- `-f (path/to/file)` Provide a [particle system file](#particle-system-file).

The default arguments are:
```bash
./N-body -v 1 -i 2 -n 100 -t 0.0035 -s 40.0
```
Alternative usage:
```bash
./N-body -version 1 -init 2 -n 100 -time-step 0.0035 -softening 40.0
```

## Available Versions

These are the available versions you can try:

| Version | Description                                                                                                                                                                                                                                       | Time Complexity                                                   |
|---------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------|
| `-v 1`  | Naïve particle-particle interaction computed sequentially on the CPU. Every particle interacts with every other particle directly.                                                                                                                | O(n^2)                                                            |
| `-v 2`  | Parallelized version of `-v 1` using OpenMP for CPU multithreading, improving performance.                                                                                                                            | O(n^2)                                                            |
| `-v 3`  | GPU-accelerated particle-particle interaction using compute shaders. Leverages massive parallelism but still follows a brute-force approach.                                                                                                      | O(n^2)                                                            |
| `-v 4`  | Optimized version of `-v 3` based on [NVIDIA's Fast N-body Simulation](https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-31-fast-n-body-simulation-cuda), reducing memory bottlenecks and improving GPU efficiency. | O(n^2)                                                            |
| `-v 5`  | Iterative, stackless Octree Barnes-Hut algorithm computed sequentially on the CPU. Approximates distant particle groups as single bodies to reduce complexity.                                                                                    | O(n log n)                                                        |
| `-v 6`  | Fully parallel CPU Barnes-Hut implementation. Both tree construction and force calculations are parallelized using OpenMP.                                                                                                                        | O(n log n)                                                        |
| `-v 7` | Fully parallel GPU Barnes-Hut implementation. Both tree construction and force calculations are performed on the GPU for maximum parallelism.                                                                                                     | O(n log n)                                                        |

**Important Note on Barnes-Hut Versions (-v 5, -v 6, -v 7):**

These implementations have known stability issues and bugs. They are prone to crashing, especially with large particle counts (e.g., >700k) or when particles approach each other very closely. The performance may also be suboptimal compared to theoretical expectations. Due to the significant difficulty in debugging these complex parallel algorithms, these issues are unlikely to be fixed. For stable simulations, please use the brute-force versions (-v 1 to -v 4). 
> If you want to learn how it works read `BarnesHutExplained.md`

## Available initializations
You can try the next initializations:
- `-i 1` Particles form a cube.
- `-i 2` Particles form a disk.
- `-i 3` Particles form an equilateral triangle. (Only 3 particles)
- `-i 4` Particles form a sphere. (Only the surface)
- `-i 5` Particles form a ball.
- `-i 6` Particles form a cube. (Only the surface)

## Particle system file
The files you give to the program must follow this format:
```html
Particle System with 3 particles:
Particle ID: 0
Position: (5.8337, 4.6008, 3.35599)
Velocity: (-1.15428, 1.8317, 0)
Acceleration: (0, 0, 0)
Mass: 0.625
Particle ID: 1
Position: (2.13544, 2.37607, 3.12727)
Velocity: (1.01254, -1.94254, 0)
Acceleration: (0.4, 3.0, 0)
Mass: 0.875
Particle ID: 2
Position: (2.74389, 2.46503, 3.42229)
Velocity: (0.0960527, 2.17343, 0)
Acceleration: (0, 0, -8.0)
Mass: 0.875
Particle ID: 3
Position: (3.38766, 0.977181, 2.78489)
Velocity: (1.10174, 2.40912, 0)
Acceleration: (-8.0, 0, 0)
Mass: 0.625
```
> Note: The world dimensions are (5, 5, 5)

This is how you can use the `-f` argument:
````bash
./N-body -f path/to/file
````

## Program structure
Here you can see the class diagram. It should be relativley easy to add new initializations and versions.  

![Class diagram](media/classdiagram.svg.svg)


An example of how a new simulation version can be added is by creating a new class that implements the `ParticleSolver` interface and then including the new version in the list of available version options in the `ArgumentsParser` class. This way, the user can select the new version through the input arguments.   

Similarly, to add a new initialization, a new class that implements the `ParticleSystemInitializer` interface can be created and the new option can be added to the list of initialization options in the `ArgumentsParser` class.   

Finally, you would have to use the new initalization or version you created in `main.cpp`.
