

#include "ParticleDrawer.h"


ParticleDrawer::~ParticleDrawer() {
    delete this->renderShader;
    delete this->finalRenderShader;
    delete this->bloom;
    delete this->camera;
}
ParticleDrawer::ParticleDrawer(glm::vec3 worldDim, glm::vec2 windowDim) {
    this->camera = new Camera(windowDim, worldDim);
    this->bloom = new Bloom(windowDim);

    this->renderShader = new VertexFragmentShader("../src/shaders/vertexShader.glsl", "../src/shaders/fragmentShader.glsl");
    this->renderShader->use();
    this->renderShader->setFloat("worldSize", glm::length(worldDim));
    this->pointSize = false;


    this->finalRenderShader = new VertexFragmentShader("../src/shaders/finalRender_vs.glsl", "../src/shaders/finalRender_fs.glsl");
    this->finalRenderShader->use();
    this->finalRenderShader->setInt("normalScene", 0);
    this->finalRenderShader->setInt("blurScene", 1);
    this->finalRenderShader->setFloat("intensity", this->bloom->getIntensity());


    // Screen texture quad VAO
    glGenVertexArrays(1, &this->screenVAO);
    glGenBuffers(1, &this->screenVBO);
    glBindVertexArray(this->screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->screenVBO);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(this->frameBufferTexture), this->frameBufferTexture, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)offsetof(Texture, texCoords));
}

void ParticleDrawer::draw(size_t particlesCount, const float dt) {
    if(!this->bloom->isActivated()){
        // Use the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        this->drawNormalScene(particlesCount);
    }
    else{
        this->drawBloomScene(particlesCount);
    }
    this->camera->update(dt);
}

void ParticleDrawer::drawNormalScene(size_t particlesCount) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->renderShader->use();
    this->renderShader->setMat4("modelViewProjection", this->camera->getModelViewProjection());
    this->renderShader->setVec3("cameraPos", this->camera->getPosition());
    this->renderShader->setBool("pointSize", this->pointSize);
	this->renderShader->setInt("totalParticles", particlesCount);
    glDrawArrays(GL_POINTS, 0, particlesCount);
}

void ParticleDrawer::drawBloomScene(size_t particlesCount) {

    // Render the normal scene and extract the bright particles
    this->bloom->useFrameBuffer();
    this->drawNormalScene(particlesCount);

    // Blur bright particles with two-pass Gaussian Blur
    this->blurBrightParticles();

    // Render the final scene by combining the normal scene and the blurred scene
    this->combineBlurAndNormalScene();
}

void ParticleDrawer::blurBrightParticles() {
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 2;
    this->bloom->getBlurShader()->use();
    for (unsigned int i = 0; i < amount; i++)
    {
        this->bloom->bindPingPongBuffer(horizontal);
        this->bloom->getBlurShader()->setInt("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? this->bloom->getBrightPointsScene() : this->bloom->getPingPongTexture(!horizontal));  // bind texture of other framebuffer (or scene if first iteration)
        glBindVertexArray(this->screenVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        horizontal = !horizontal;
        if (first_iteration){
            first_iteration = false;
        }
    }
}

void ParticleDrawer::combineBlurAndNormalScene() {
    // Use the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // disable depth test so screen-space quad isn't discarded due to depth test.
    glDisable(GL_DEPTH_TEST);

    // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    this->finalRenderShader->use();
    this->finalRenderShader->setFloat("intensity", this->bloom->getIntensity());

    glBindVertexArray(this->screenVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->bloom->getNormalScene());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->bloom->getPingPongTexture(false));	// use the color attachment texture as the texture of the quad plane

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Camera* ParticleDrawer::getCamera() {
    return this->camera;
}

void ParticleDrawer::updateWindowDimension(glm::vec2 window) {
    this->camera->setAspectRatio(window);
    delete this->bloom;
    this->bloom = new Bloom(window);
}

Bloom *ParticleDrawer::getBloom() {
    return this->bloom;
}

bool ParticleDrawer::getPointSize() {
    return this->pointSize;
}

void ParticleDrawer::setPointSize(bool point) {
    this->pointSize = point;
}