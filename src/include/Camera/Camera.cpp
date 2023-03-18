//
// This code is not 100% mine, although is modified by me (marc) on 15/03/2023
// The code was written with the help of ChatGPT
//

#include "Camera.h"

//====================================================
// Constructor
//====================================================

/**
 * Constructor, sets default values for camera properties
 * @param window
 * @param worldDim
 */
Camera::Camera(glm::vec2 window, glm::vec3 worldDim) {
    this->worldDimensions = worldDim;
    this->isDragging = false;
    this->prevMousePos = glm::vec2(0.0f);

    // The initial position of the camera in a 3D space
    this->position = glm::vec3(worldDimensions.x / 2.0f, worldDimensions.y / 2.0f, (this->worldDimensions.x + this->worldDimensions.y + this->worldDimensions.z) / 2.f);

    // The center of the world that the camera is looking at
    this->worldCenter = glm::vec3(worldDimensions.x / 2.0f, worldDimensions.y / 2.0f, worldDimensions.z / 2.f);

    // The up vector of the camera, used for determining orientation
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);

    // The sensitivity of the camera when moving it with the mouse
    this->sensitivity = 0.005f;

    // The field of view angle of the camera in degrees
    this->fov = 90.0f;

    // The aspect ratio of the camera (width divided by height)
    this->aspectRatio = static_cast<float>(window.x) / static_cast<float>(window.y);

    // The distance from the camera to the near clipping plane, used for culling objects that are too close
    this->nearClipPlane = 0.1f;

    // The distance from the camera to the far clipping plane, used for culling objects that are too far away
    this->farClipPlane = worldDimensions.z * 4.0f;
}
//====================================================


//====================================================
// Setters
//====================================================

/**
 * Set aspect ratio
 * @param winWidth
 * @param winHeight
 */
void Camera::setAspectRatio(unsigned int winWidth, unsigned int winHeight) {
    this->aspectRatio = static_cast<float>(winWidth) / static_cast<float>(winHeight);
}

/**
 * Set isDragging (called when the user clicks or releases the mouse left button)
 * @param dragging
 */
void Camera::setIsDragging(bool dragging) {
    this->isDragging = dragging;
}

/**
 * Set previous mouse position (called when the user clicks)
 * @param prevPos
 */
void Camera::setPreviousMousePos(glm::vec2 prevPos) {
    this->prevMousePos = prevPos;
}
//====================================================


//====================================================
// Getters
//====================================================

/**
 * Get the view matrix == Camera
 * @return
 */
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(this->position, this->worldCenter, this->up);
}

/**
 * Get the projection matrix.
 * @return
 */
glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(this->fov), this->aspectRatio, this->nearClipPlane, this->farClipPlane);
}

/**
 * Get the model matrix
 * @return
 */
glm::mat4 Camera::getModelMatrix() const{
    return glm::mat4(1.f);
}

/**
 * Get the model view projection matrix
 * @return
 */
glm::mat4 Camera::getModelViewProjection() const {
    return this->getProjectionMatrix() * this->getViewMatrix() * this->getModelMatrix();
}

//====================================================


//====================================================
// Callback functions
//====================================================

/**
 * Zoom the camera in/out
 * @param deltaTime
 * @param mouseWheelYOffset
 */
void Camera::zoomCallback(float deltaTime, float mouseWheelYOffset) {
    // Define the new world dimensions based on the mouse wheel offset
    const float zoomFactor = 1.0f + mouseWheelYOffset * 0.1f;
    glm::vec3 newWorldDimensions = this->worldDimensions * zoomFactor;

    // Define the camera speed based on the ratio between the old and new world dimensions
    const float cameraSpeed = 0.04f * glm::length(newWorldDimensions) / glm::length(this->worldDimensions);

    // Update the camera position based on the mouse wheel offset and camera speed
    if (mouseWheelYOffset > 0.f) {
        // Zoom in
        // Update the position of the camera
        this->position += cameraSpeed * (this->worldCenter - this->position);
    } else {
        // Zoom out
        // Update camera position
        this->position -= cameraSpeed * (this->worldCenter - this->position);
    }

    // Update near and far clip planes based on camera position
    float distanceToCenter = glm::distance(this->position, this->worldCenter);
    this->nearClipPlane = distanceToCenter * 0.1f;
    this->farClipPlane = distanceToCenter * 10.0f;

    // Update the world dimensions
    this->worldDimensions = newWorldDimensions;
}

/**
 * Rotate the camera based on mouse dragging movement
 * @param mousePos
 */
void Camera::rotateCallback(glm::vec2 mousePos) {
    if (!isDragging) {
        // The user is not dragging
        return;
    }

    // Calculate the difference in mouse position
    glm::vec2 delta = glm::vec2(mousePos.x, mousePos.y) - this->prevMousePos;
    this->prevMousePos = glm::vec2(mousePos.x, mousePos.y);

    // Calculate the rotation angles
    float pitch = delta.y * this->sensitivity;
    float yaw = -delta.x * this->sensitivity;

    // Calculate the direction vector and right vector
    glm::vec3 direction = glm::normalize(worldCenter - position);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));

    // Rotate around the up vector
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), yaw, up);
    direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));
    right = glm::normalize(glm::cross(direction, up));
    rotation = glm::rotate(glm::mat4(1.0f), pitch, right);
    direction = glm::vec3(rotation * glm::vec4(direction, 0.0f));

    // Update the camera position and world center
    position = worldCenter - direction * glm::length(worldCenter - position);
    up = glm::normalize(glm::cross(right, direction));
}
//====================================================
