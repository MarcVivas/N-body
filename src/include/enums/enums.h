#ifndef N_BODY_ENUMS_H
#define N_BODY_ENUMS_H
enum InitializationType{
    CUBE = 1,
    GALAXY = 2,
    LAGRANGE = 3,
    SPHERE = 4,
    BALL = 5,
    CUBE_SURFACE = 6
};

enum Version {
    PP_CPU_SEQUENTIAL = 1,
    PP_CPU_PARALLEL = 2,
    PP_GPU_PARALLEL = 3,
    PP_GPU_OPTIMIZED = 4,
    GRID_CPU = 5,
    GRID_GPU = 6
};

#endif //N_BODY_ENUMS_H
