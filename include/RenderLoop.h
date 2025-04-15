

#ifndef N_BODY_RENDERLOOP_H
#define N_BODY_RENDERLOOP_H
#include "Window.h"
#include "ParticleSimulation.h"
#include "RenderTimer.h"
#include <memory>

class RenderLoop {
public:
    RenderLoop();
    RenderLoop(const Window& win, bool showFps, bool vSync);
    void runLoop(std::shared_ptr<ParticleSimulation> particleSimulation);
    void setPauseSimulation(bool pause);
    bool getPauseSimulation() {
        return this->pauseSimulation;
    }
    int getIteration();
    ~RenderLoop();
private:
    Window window;
    RenderTimer renderTimer;
    bool pauseSimulation;
};


#endif //N_BODY_RENDERLOOP_H
