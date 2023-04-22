#include <glm/glm.hpp>
#include "VertexFragmentShader.h"

#ifndef N_BODY_BLOOM_H
#define N_BODY_BLOOM_H


class Bloom {
public:
    ~Bloom();
    Bloom(glm::vec2 &windowDim);
    void useFrameBuffer();
    unsigned int getNormalScene();
    unsigned int getBrightPointsScene();
    unsigned int getPingPongTexture(unsigned int pos);
    bool isActivated();
    void setIsActive(const bool active);
    float getIntensity();
    void setIntensity(float newIntensity);
    void bindPingPongBuffer(unsigned int pos);
    VertexFragmentShader* getBlurShader();
private:
    unsigned int hdrFrameBuffer;
    unsigned int renderBuffer;
    unsigned int scene[2];
    unsigned int sceneTextures[2];
    unsigned int pingpongFBO[2];
    unsigned int pingPongTextures[2];
    float intensity;
    bool activated;
    VertexFragmentShader *blurShader;
};


#endif //N_BODY_BLOOM_H
