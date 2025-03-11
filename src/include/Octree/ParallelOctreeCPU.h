#include <iostream>
#include <omp.h>

#include "Octree.h"

#ifndef PARALLELOCTREECPU_H
#define PARALLELOCTREECPU_H


struct Task {
    int root;
    int *particles;
    int totalParticles;
    int max;
    // Default Constructor
    Task() : root(-1), particles(nullptr), totalParticles(0), max(0) {}
    Task(int n) {
        root = -1;
        max = n;
        particles = new int[max];
        totalParticles = 0;
    }

    void insert(int root, int particle) {
        this->root = root;
        if (totalParticles < max) {
            this->particles[totalParticles] = particle;
            totalParticles++;
        }
        else {

            int newMax = max * 2;  // Only multiply once
            int* new_particles = new int[newMax];  // Allocate new array

            for (int i = 0; i < totalParticles; i++) {
                new_particles[i] = particles[i];  // Copy existing data
            }

            delete[] particles;  // Free old memory safely
            particles = new_particles;  // Assign new memory
            max = newMax;  // Update max size

            // Now add the new particle
            particles[totalParticles] = particle;
            totalParticles++;
        }

    }

    // Deep copy constructor
    Task(const Task& other)
        : root(other.root), totalParticles(other.totalParticles), max(other.max) {
        particles = new int[max];
        for (int i = 0; i < totalParticles; i++) {
            particles[i] = other.particles[i];
        }
    }

    // Deep copy assignment operator
    Task& operator=(const Task& other) {
        if (this != &other) {
            delete[] particles;
            root = other.root;
            totalParticles = other.totalParticles;
            max = other.max;
            particles = new int[max];
            for (int i = 0; i < totalParticles; i++) {
                particles[i] = other.particles[i];
            }
        }
        return *this;
    }
    ~Task() {
        delete[] particles;
    }
    void reset() {
        root = -1;
        totalParticles = 0;
    }
};

class ParallelOctreeCPU: public Octree {
public:
    ParallelOctreeCPU(int n);
    virtual ~ParallelOctreeCPU();
    void insert(ParticleSystem *p);
    virtual glm::vec4 computeGravityForce(glm::vec4& pos, float squaredSoftening, float G);
    void computeGravityForces(ParticleSystem *p, float squaredSoftening, float G);

    Octree **octrees;
    Task *tasks;
    int totalTasks;

    int maxDepth;
    int numThreads;
    std::vector<omp_lock_t> nodeLocks;

    private:
        void subdivide(int i, int offset);
        void resetArrays(int totalParticles, int totalNodes);
        void countParticlesPerNode(int particleCount);
        void copyActiveParticles(int particleCount);
        void assignParticlesToNodes(int particleCount, ParticleSystem *p);
        void classifyNodes(int firstNode);
        void subdivideNodes(int &firstNode);
        void prepareParticlesForNextIteration(int &particleCount);
        void processParticlesForNextIteration(int particleCount, ParticleSystem *p);
};



#endif //PARALLELOCTREECPU_H
