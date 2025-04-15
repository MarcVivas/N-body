

#ifndef N_BODY_WINDOWINPUTMANAGER_H
#define N_BODY_WINDOWINPUTMANAGER_H
#include "Window.h"
#include "ParticleSimulation.h"
#include "RenderLoop.h"
#include <memory>
class WindowInputManager {
public:
    WindowInputManager(Window *win, RenderLoop *loop, std::shared_ptr<ParticleSimulation> particleSimulation);
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
    std::shared_ptr<ParticleSimulation> getParticleSimulation() {
		return this->particleSimulation;
    }
private:
    Window *window;
    RenderLoop *renderLoop;
	std::shared_ptr<ParticleSimulation> particleSimulation;
};


#endif //N_BODY_WINDOWINPUTMANAGER_H
