#include "VertexFragmentShader.h"

VertexFragmentShader::VertexFragmentShader(std::string vertPath, std::string fragPath): Shader() {
     // Load vertex shader source code from file
    std::string vertexCode = this->readFileToString(vertPath);

    // Load fragment shader source code from file
    std::string fragmentCode = this->readFileToString(fragPath);

    // Compile shaders
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode.c_str());
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

    // Create the shader program and attach shaders
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    glLinkProgram(this->ID);
    this->checkCompileErrors(this->ID, "PROGRAM");

    // Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}