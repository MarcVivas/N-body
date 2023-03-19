//
// Created by marc on 18/03/23.
//

#ifndef N_BODY_RENDERTIMER_H
#define N_BODY_RENDERTIMER_H


class RenderTimer {
public:
    RenderTimer(bool showFPS, bool vSync);
    void setDeltaTime(double time);
    double getDeltaTime();
    void updateTime(Window& window);
private:
    double deltaTime;
    bool showFPS;
    double previousTime;
    size_t frameCount;
    double lastFrameTime;
    void updateFPS(Window& window);
    void updateDeltaTime();
};


#endif //N_BODY_RENDERTIMER_H
