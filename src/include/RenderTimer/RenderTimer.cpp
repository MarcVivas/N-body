//
// Created by marc on 18/03/23.
//

#include "RenderTimer.h"

RenderTimer::RenderTimer(bool showFps, bool vSync) :showFPS(showFps), previousTime(glfwGetTime()), frameCount(0), lastFrameTime(glfwGetTime()){
    if(!vSync){
        // Disable Vsync
        glfwSwapInterval(0);
    }
}

double RenderTimer::getDeltaTime() {
    return this->deltaTime;
}

void RenderTimer::setDeltaTime(double time) {
    this->deltaTime = time;
}

void RenderTimer::updateFPS(Window& window) {
    double currentTime = glfwGetTime();
    this->frameCount++;

    double elapsedTime = currentTime - this->previousTime;

    // If elapsed time is greater than or equal to 1 second
    if (elapsedTime >= 1.0) {

        // Compute the fps and the average frame time
        double fps = this->frameCount / elapsedTime;
        double ms_per_frame = 1000.0 / fps;

        if (this->showFPS) {
            // Update the window title
            std::ostringstream title;
            title << "N-body simulation | " << ms_per_frame << " ms/frame (" << fps << " FPS)";
            window.updateWindowTitle(title.str().c_str());
        }

        // Reset for the next update
        this->previousTime = currentTime;
        this->frameCount = 0;
    }
}

void RenderTimer::updateDeltaTime(){
    double currentTime = glfwGetTime();
    this->setDeltaTime(currentTime - this->lastFrameTime);
    this->lastFrameTime = currentTime;
}

void RenderTimer::updateTime(Window &window) {
    this->updateFPS(window);
    this->updateDeltaTime();
}

