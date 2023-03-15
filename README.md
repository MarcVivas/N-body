# N-body simulation


## Table of contents
[Controls](#controls)
[How to run the project](#how-to-run-the-project)

## Controls
| Key | Action                       |
|-----|------------------------------|
| Esc | Closes the program           |
 | p   | Pauses/starts the simulation | 




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


