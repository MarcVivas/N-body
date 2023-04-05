//
// Created by marc on 5/04/23.
//

#include "../abstract/Shader.h"

#ifndef N_BODY_COMPUTESHADER_H
#define N_BODY_COMPUTESHADER_H


class ComputeShader: public Shader {
public:
    ComputeShader(std::string &computeShaderPath);

};


#endif //N_BODY_COMPUTESHADER_H
