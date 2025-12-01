#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera Movement 
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


// Default Camera Intristics 
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f; 
const float ZOOM = 45.0f;

class Camera {
public:
    // Camera attributes 
    glm::vec3 Position;
    glm::vec3 Front; 
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles 
    float Yaw;
    float Pitch; 

    // Camera options 
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom; 

    // Constructor with Vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch= PITCH);
    // Constructor with scalar values 
    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const; 
    
    // Returns the projection matrix
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    // Processes input received from any keyboard-like input system
    void processKeyboard(Camera_Movement direction, float deltaTime);

    // Processes input received from a mouse input system
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Prcocesses input recieved from a mouse scroll-wheel event
    void processMouseScroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors(); 
};
#endif // CAMERA_H