#include <glad/glad.h>
#include <glm/glm.hpp>
#include "VertexFragmentShader.h"
#include "Camera.h"
#include "Bloom.h"


#ifndef N_BODY_PARTICLEDRAWER_H
#define N_BODY_PARTICLEDRAWER_H

struct Texture {
    glm::vec3 position;
    glm::vec2 texCoords;
};


class ParticleDrawer {
public:
    ~ParticleDrawer();
    ParticleDrawer(glm::vec3 worldDim, glm::vec2 windowDim);
    void draw(size_t particlesCount, float dt);
    Camera* getCamera();
    void updateWindowDimension(glm::vec2 window);
    Bloom* getBloom();
    bool getPointSize();
    void setPointSize(bool point);
protected:
    GLuint screenVAO, screenVBO;
    Shader *renderShader, *finalRenderShader;
    Bloom *bloom;
    Camera *camera;
    bool pointSize;

    Texture frameBufferTexture[4] = {
            {glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(1.0f,  1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
    };

    void drawBloomScene(size_t particlesCount);
    void drawNormalScene(size_t particlesCount);
    void blurBrightParticles();
    void combineBlurAndNormalScene();
};


#endif //N_BODY_PARTICLEDRAWER_H
