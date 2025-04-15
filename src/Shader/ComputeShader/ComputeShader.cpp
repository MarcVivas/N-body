#include "ComputeShader.h"


ComputeShader::ComputeShader(std::string &computeShaderPath): Shader(){
    // Load vertex shader source code from file
    std::string code = this->readFileToString(computeShaderPath);

    // Compile shader
    unsigned int shader = compileShader(GL_COMPUTE_SHADER, code.c_str());

    // Create the shader program and attach shaders
    this->ID = glCreateProgram();
    glAttachShader(this->ID, shader);
    glLinkProgram(this->ID);
    this->checkCompileErrors(this->ID, "PROGRAM");

    // Delete shaders
    glDeleteShader(shader);
}