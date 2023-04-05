//
// Created by marc on 5/04/23.
//

#include "../abstract/Shader.h"


#ifndef N_BODY_VERTEXFRAGMENTSHADER_H
#define N_BODY_VERTEXFRAGMENTSHADER_H

class VertexFragmentShader: public Shader{
public:
    VertexFragmentShader(std::string vertexPath, std::string fragmentPath);

};


#endif //N_BODY_VERTEXFRAGMENTSHADER_H
