#include "enums.h"
#include <cstddef>

#ifndef N_BODY_ARGUMENTSPARSER_H
#define N_BODY_ARGUMENTSPARSER_H


class ArgumentsParser {
public:
    ArgumentsParser(int argc, char *argv[]);
    Version getVersion();
    InitializationType getInitializationType();
    size_t getNumParticles();
    bool isTest();
    bool isBenchmark();
private:
    Version version;
    InitializationType init;
    size_t numParticles;
    bool test;
    bool benchmark;
};


#endif //N_BODY_ARGUMENTSPARSER_H
