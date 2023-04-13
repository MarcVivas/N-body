//
// Created by marc on 18/03/23.
//

#ifndef N_BODY_RENDERLOOP_H
#define N_BODY_RENDERLOOP_H


class RenderLoop {
public:
    RenderLoop(const Window& win, bool showFps, bool vSync);
    void runLoop(ParticleSimulation *particleSimulation);
    void setPauseSimulation(bool pause);
    bool getPauseSimulation();
    ~RenderLoop();
private:
    Window window;
    RenderTimer renderTimer;
    bool pauseSimulation;
};


#endif //N_BODY_RENDERLOOP_H
