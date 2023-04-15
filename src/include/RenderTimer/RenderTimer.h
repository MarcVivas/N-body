//
// Created by marc on 18/03/23.
//

#ifndef N_BODY_RENDERTIMER_H
#define N_BODY_RENDERTIMER_H


class RenderTimer {
public:
    RenderTimer();
    RenderTimer(bool showFPS, bool vSync);
    void setDeltaTime(double time);
    double getDeltaTime();
    void updateTime(Window& window, bool paused);
    void printFinalStats();
    double getTotalElapsedTime();
private:
    double deltaTime;
    bool showFPS;
    double previousTime;
    size_t frameCount;
    size_t totalIterations;
    double totalElapsedTime;
    double lastFrameTime;
    void updateFPS(Window& window);
    void updateDeltaTime(bool paused);
};


#endif //N_BODY_RENDERTIMER_H
