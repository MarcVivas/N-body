//
// Created by marc on 2/04/23.
//

#ifndef N_BODY_BLOOM_H
#define N_BODY_BLOOM_H


class Bloom {
public:
    Bloom(glm::vec2 &windowDim);
private:
    unsigned int hdrFrameBuffer;
    unsigned int attachments[2];
};


#endif //N_BODY_BLOOM_H
