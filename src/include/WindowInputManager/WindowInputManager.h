#include "Window.h"
#include "ParticleSimulation.h"
#include "RenderLoop.h"

#ifndef N_BODY_WINDOWINPUTMANAGER_H
#define N_BODY_WINDOWINPUTMANAGER_H


class WindowInputManager {
public:
    WindowInputManager(Window *win, RenderLoop *loop, ParticleSimulation* particleSimulation);
    // Callbacks
    void setKeyCallback();
    void setMouseButtonCallback();
    // Resizeable window
    void setFramebufferSizeCallback();
    // Cursor movement
    void setMouseMovementCallback();
    // Mouse scroll callback
    void setScrollCallback();

    RenderLoop *getRenderLoop();
    Window *getWindow();
    ParticleSimulation *getParticleSimulation();
private:
    Window *window;
    RenderLoop *renderLoop;
    ParticleSimulation *particleSimulation;
};


#endif //N_BODY_WINDOWINPUTMANAGER_H
