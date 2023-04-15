//
// Created by marc on 18/03/23.
//

#include "RenderLoop.h"

RenderLoop::RenderLoop(const Window& win, bool showFps, bool vSync) :
    window(win),
    renderTimer(RenderTimer(showFps, vSync)),
    pauseSimulation(true),
    isBenchmark(false)
{}

RenderLoop::RenderLoop()=default;

void RenderLoop::runLoop(ParticleSimulation *particleSimulation) {
    this->pauseSimulation = !this->isBenchmark;

    while (!glfwWindowShouldClose(this->window.getWindow()))
    {
        if(this->isBenchmark){
            if (this->renderTimer.getTotalElapsedTime() >= 4.0){
                break;
            }
        }

        this->renderTimer.updateTime(this->window, this->pauseSimulation);

        if(!this->pauseSimulation){
            particleSimulation->update(this->renderTimer.getDeltaTime());
        }

        particleSimulation->draw();

        // ======================
        // Swap buffers: Front buffer(render) and back buffer (next render)
        glfwSwapBuffers(this->window.getWindow());
        // ======================

        // ======================
        // Checks if any events are triggered (keys pressed/released, mouse moved etc.) and calls the corresponding functions
        glfwPollEvents();
        // ======================

    }
}

void RenderLoop::setPauseSimulation(bool pause) {
    this->pauseSimulation = pause;
}

bool RenderLoop::getPauseSimulation(){
    return this->pauseSimulation;
}


RenderLoop::~RenderLoop(){
    this->renderTimer.printFinalStats();
}

void RenderLoop::setIsBenchmark(bool bench) {
    this->isBenchmark = bench;
}

