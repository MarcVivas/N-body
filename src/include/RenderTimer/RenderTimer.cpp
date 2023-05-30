#include "RenderTimer.h"
#include <iostream>
#include <sstream>

RenderTimer::RenderTimer() = default;

RenderTimer::RenderTimer(bool showFps, bool vSync) :
    showFPS(showFps),
    previousTime(glfwGetTime()),
    frameCount(0),
    totalElapsedTime(0), 
    totalIterations(0),
    lastFrameTime(glfwGetTime()){
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

void RenderTimer::updateDeltaTime(bool paused){
    double currentTime = glfwGetTime();
    this->setDeltaTime(currentTime - this->lastFrameTime);
    this->lastFrameTime = currentTime;

    if(!paused){
        this->totalElapsedTime += this->deltaTime;
        this->totalIterations += 1;
    }

}

void RenderTimer::updateTime(Window &window, bool paused) {
    this->updateFPS(window);
    this->updateDeltaTime(paused);

}

/**
 *   
 *  Print the final average frame time
*/  
void RenderTimer::printFinalStats() {
    std::cout << "===============================================\n";
    std::cout << "Simulation performance stats\n"; 
    std::cout << "===============================================\n";
    

    if (totalIterations > 0) {
        double averageFrameTime = totalElapsedTime / totalIterations;
        double averageFPS = 1.0 / averageFrameTime;

        std::cout << "Total elapsed time: " << totalElapsedTime  << " seconds" << std::endl;
        std::cout << "Average FPS: " <<  averageFPS << " fps" << std::endl;
        std::cout << "Average Frame Time: " << averageFrameTime * 1000 << " ms" << std::endl;
    }     
    std::cout << "Total iterations: " << this->totalIterations << " iterations" << std::endl;
    std::cout << "===============================================\n";

}

int RenderTimer::getIteration() {
    return this->totalIterations;
}

double RenderTimer::getTotalElapsedTime() {
    return this->totalElapsedTime;
}

