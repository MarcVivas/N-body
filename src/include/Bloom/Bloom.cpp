//
// Created by marc on 2/04/23.
//

#include "Bloom.h"

Bloom::Bloom(glm::vec2 &windowDim) {

    // set up floating point framebuffer to render scene to
    glGenFramebuffers(1, &this->hdrFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFrameBuffer);
    unsigned int colorBuffers[2];

    // Create 2 textures for the frame buffer object
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RGBA16F, windowDim.x, windowDim.y, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
        );
    }

    // Tell OpenGL that we are using 2 color attachments for this frameBuffer
    this->attachments[0] = GL_COLOR_ATTACHMENT0;
    this->attachments[1] = GL_COLOR_ATTACHMENT1;
    glDrawBuffers(2, this->attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}