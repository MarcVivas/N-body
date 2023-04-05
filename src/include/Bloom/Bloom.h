//
// Created by marc on 2/04/23.
//

#ifndef N_BODY_BLOOM_H
#define N_BODY_BLOOM_H


class Bloom {
public:
    Bloom(glm::vec2 &windowDim, float exposure, int bloomAmount);
    void useFrameBuffer();
    unsigned int getNormalScene();
    unsigned int getBrightPointsScene();
    unsigned int getPingPongFrameBuffer(unsigned int pos);
    unsigned int getPingPongTexture(unsigned int pos);
    int getBloomAmount();
    float getExposure();
    void bindPingPongBuffer(unsigned int pos);
private:
    unsigned int hdrFrameBuffer;
    unsigned int renderBuffer;
    unsigned int attachments[2];
    unsigned int sceneTextures[2];
    unsigned int pingpongFBO[2];
    unsigned int pingPongTextures[2];
    float exposure;
    int bloomAmount;
};


#endif //N_BODY_BLOOM_H
