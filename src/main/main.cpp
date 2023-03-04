#include "../../lib/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include "../include/OpenGLRenderer/OpenGLRenderer.cpp"

int main()
{
    OpenGLRenderer renderer(600, 600, "N-body simulation", false, true);
    renderer.render_loop([]()->void{
        // Update

    }, []()->void{
        // Draw
    });

}
