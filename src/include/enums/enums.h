#ifndef N_BODY_ENUMS_H
#define N_BODY_ENUMS_H
enum InitializationType{
    CUBE = 1,
    GALAXY = 2,
    LAGRANGE = 3,
    SPHERE = 4,
    BALL = 5,
    CUBE_SURFACE = 6,
    SYSTEM_FILE = 7
};

enum Version {
    PP_CPU_SEQUENTIAL = 1,
    PP_CPU_PARALLEL = 2,
    PP_GPU_PARALLEL = 3,
    PP_GPU_OPTIMIZED = 4,
    GRID_CPU = 5,
    BARNES_HUT_CPU_SEQ = 6,
    BARNES_HUT_CPU_PARALLEL_HYBRID = 7,
    BARNES_HUT_GPU_PARALLEL_HYBRID = 8
};

#endif //N_BODY_ENUMS_H
