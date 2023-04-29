
#include <cmath>
#include "ParticleSystem.h"

#ifndef N_BODY_ENERGYCALCULATOR_H
#define N_BODY_ENERGYCALCULATOR_H


class EnergyCalculator {
public:
    EnergyCalculator(float stepSize, float softening);
    void computeEnergy(ParticleSystem *particles);
    float stepSize;
    float softening;

};


#endif //N_BODY_ENERGYCALCULATOR_H
