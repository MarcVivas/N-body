# N-body
N-body simulation

## How to run the project

In this chapter you will find what I did to run the project in the next OS.

1. [Ubuntu](#ubuntu)
2. [Arch Linux](#arch-linux)

### Ubuntu


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
##### 5. Run the project.  

Do the next step only once. This command will create a new directory titled `build` and change the location to it.
```bash
mkdir build && cd build
```

Build a `Makefile` using CMake.
```bash
cmake ..
```
Compile the program using the `Makefile`.
```bash
make
```

Now you should be able to run the compiled program.
```bash
./N-body
```


