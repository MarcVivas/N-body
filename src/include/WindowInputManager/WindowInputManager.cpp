//
// Created by marc on 18/03/23.
//

#include "WindowInputManager.h"

WindowInputManager::WindowInputManager(Window *win, RenderLoop *loop, ParticleSimulation* particleSim): window(win), renderLoop(loop), particleSimulation(particleSim) {
    this->setKeyCallback();
    this->setFramebufferSizeCallback();
    this->setMouseMovementCallback();
    this->setMouseButtonCallback();
    this->setScrollCallback();
}

RenderLoop* WindowInputManager::getRenderLoop() {
    return this->renderLoop;
}

Window* WindowInputManager::getWindow(){
    return this->window;
}

ParticleSimulation* WindowInputManager::getParticleSimulation(){
    return this->particleSimulation;
}



/**
 * Set callback functions for when relevant keys are pressed/released to react accordingly
 */
void WindowInputManager::setKeyCallback() {
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetKeyCallback(this->window->getWindow(), [](GLFWwindow* win, int key, int scancode, int action, int mods) -> void
                       {
                           auto inputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance
                           if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
                               glfwSetWindowShouldClose(win, true);
                           }
                           if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
                               inputManager->getRenderLoop()->setPauseSimulation(!inputManager->getRenderLoop()->getPauseSimulation());
                           }

                           if( key == GLFW_KEY_B && action == GLFW_PRESS){
                               auto bloom = inputManager->getParticleSimulation()->getParticleDrawer()->getBloom();
                               bloom->setIsActive(!bloom->isActivated());
                           }

                           if( key == GLFW_KEY_D   && action == GLFW_PRESS ){
                               auto bloom = inputManager->getParticleSimulation()->getParticleDrawer()->getBloom();
                               float decrease = 0.1;
                               bloom->setIntensity(bloom->getIntensity() - decrease < 0 ? 0.f : bloom->getIntensity() - decrease);
                               std::cout << "Intensity: " << bloom->getIntensity() << '\n';
                           }
                           if( key == GLFW_KEY_I   && action == GLFW_PRESS){
                               auto bloom = inputManager->getParticleSimulation()->getParticleDrawer()->getBloom();
                               float increase = 0.1;
                               bloom->setIntensity(bloom->getIntensity() + increase);
                               std::cout << "Intensity: " << bloom->getIntensity() << '\n';
                           }
                           if( key == GLFW_KEY_Q   && action == GLFW_PRESS){
                               auto particleDraw = inputManager->getParticleSimulation()->getParticleDrawer();
                               bool pointSize = !particleDraw->getPointSize();
                               particleDraw->setPointSize(pointSize);
                           }


                       }
    );
}


/**
 * Resizeable window callback
 */
void WindowInputManager::setFramebufferSizeCallback() {
    // Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process.
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetFramebufferSizeCallback(this->window->getWindow(), [](GLFWwindow* win, int width, int height)->void
    {
        // retrieve the pointer to the instance
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win));
        // Update the window size
        windowInputManager->getWindow()->updateWindowSize(width, height);

        // Update the aspect ratio
        windowInputManager->getParticleSimulation()->getParticleDrawer()->updateWindowDimension(glm::vec2(width, height));
    });
}


// Cursor/mouse movement
void WindowInputManager::setMouseMovementCallback() {
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetCursorPosCallback(this->window->getWindow(), [](GLFWwindow* win, double xPos, double yPos)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance
        windowInputManager->getParticleSimulation()->getCamera()->rotateCallback(glm::vec2(xPos, yPos));
    });
}


void WindowInputManager::setScrollCallback(){
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetScrollCallback(this->window->getWindow(), [](GLFWwindow* win, double xOffset, double yOffset)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance
        windowInputManager->getParticleSimulation()->getCamera()->zoomCallback(yOffset);
    });
}

void WindowInputManager::setMouseButtonCallback() {
    glfwSetWindowUserPointer(this->window->getWindow(), this);
    glfwSetMouseButtonCallback(this->window->getWindow(), [](GLFWwindow* win, int button, int action, int mods)->void
    {
        auto windowInputManager = static_cast<WindowInputManager*>(glfwGetWindowUserPointer(win)); // retrieve the pointer to the instance
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            windowInputManager->getParticleSimulation()->getCamera()->setIsDragging(true);
            double prevMouseXPos, prevMouseYPos;
            glfwGetCursorPos(win, &prevMouseXPos, &prevMouseYPos);
            windowInputManager->getParticleSimulation()->getCamera()->setPreviousMousePos(glm::vec2(prevMouseXPos, prevMouseYPos));
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            windowInputManager->getParticleSimulation()->getCamera()->setIsDragging(false);
        }
    });
}
