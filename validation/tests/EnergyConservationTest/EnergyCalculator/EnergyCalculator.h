
#include <cmath>
#include "ParticleSystem.h"

#ifndef N_BODY_ENERGYCALCULATOR_H
#define N_BODY_ENERGYCALCULATOR_H


class EnergyCalculator {
public:
    EnergyCalculator() = default;
    void computeEnergy(ParticleSystem *particles, const float squaredSoftening);

};


#endif //N_BODY_ENERGYCALCULATOR_H
