#include "Window.h"
#include "ParticleSimulation.h"
#include "RenderTimer.h"

#ifndef N_BODY_RENDERLOOP_H
#define N_BODY_RENDERLOOP_H


class RenderLoop {
public:
    RenderLoop();
    RenderLoop(const Window& win, bool showFps, bool vSync);
    void runLoop(ParticleSimulation *particleSimulation);
    void setPauseSimulation(bool pause);
    bool getPauseSimulation();
    void setIsBenchmark(bool bench);
    ~RenderLoop();
private:
    Window window;
    RenderTimer renderTimer;
    bool pauseSimulation;
    bool isBenchmark;
};


#endif //N_BODY_RENDERLOOP_H
