
#include "Bloom.h"
#include <glad/glad.h>
#include <iostream>

Bloom::Bloom(glm::vec2 &windowDim) {
    this->activated = true;
    this->intensity = 0.4f;

    // set up floating point framebuffer to render scene to
    glGenFramebuffers(1, &this->hdrFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFrameBuffer);


    // Create 2 textures for the frame buffer object
    glGenTextures(2, this->sceneTextures);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, this->sceneTextures[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F, windowDim.x, windowDim.y, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->sceneTextures[i], 0);
    }

    // Tell OpenGL that we are using 2 color scene for this frameBuffer
    this->scene[0] = GL_COLOR_ATTACHMENT0;
    this->scene[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, this->scene);


    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &this->renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowDim.x, windowDim.y); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->renderBuffer); // now actually attach it
    // now that we actually created the framebuffer and added all scene we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingPongTextures);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingPongTextures[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F, windowDim.x, windowDim.y, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextures[i], 0
        );
    }

    this->blurShader = new VertexFragmentShader("../src/shaders/blur_vs.glsl", "../src/shaders/blur_fs.glsl");
    this->blurShader->use();
    this->blurShader->setInt("image", 0);
}


Bloom::~Bloom(){
    delete this->blurShader;
}

void Bloom::useFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFrameBuffer);
}


unsigned int Bloom::getNormalScene() {
    return this->sceneTextures[0];
}

unsigned int Bloom::getBrightPointsScene() {
    return this->sceneTextures[1];
}


unsigned int Bloom::getPingPongTexture(unsigned int pos) {
    return this->pingPongTextures[pos];
}

float Bloom::getIntensity() {
    return this->intensity;
}

bool Bloom::isActivated() {
    return this->activated;
}

void Bloom::bindPingPongBuffer(unsigned int pos) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBO[pos]);
}

VertexFragmentShader* Bloom::getBlurShader() {
    return this->blurShader;
}

void Bloom::setIsActive(const bool active) {
    this->activated = active;
}

void Bloom::setIntensity(float newIntensity) {
    this->intensity = newIntensity;
}