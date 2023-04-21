//
// Created by marc on 20/04/23.
//

#ifndef N_BODY_ENERGYCALCULATOR_H
#define N_BODY_ENERGYCALCULATOR_H
#include <cmath>


class EnergyCalculator {
public:
    EnergyCalculator() = default;
    void computeEnergy(ParticleSystem *particles, const float squaredSoftening);

};


#endif //N_BODY_ENERGYCALCULATOR_H
