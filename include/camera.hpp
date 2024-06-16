#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

enum CameraMovement
{
    FORWARD,
    RIGHT,
    BACKWARD,
    LEFT,
    UP,
    DOWN,
    NONE
};

class Camera
{
    public :
        Camera( glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = -45.0f, float fov = 45.0f, float sensitivity = 0.1f, float moveSpeed = 7.5f ) noexcept;

        void onKeyboard( CameraMovement direction, float deltaTime ) noexcept;
        void onMouseMove( float xOffset, float yOffset, bool constrainPitch = true ) noexcept;
        void onScroll( float offset ) noexcept;

        glm::mat4 getViewMat() const noexcept;

        float getFov() const noexcept;
        float getSensitivity() const noexcept;
        float getMoveSpeed() const noexcept;
        float getPitch() const noexcept;
        float getYaw() const noexcept;
        glm::vec3 getUpWorld() const noexcept;
        glm::vec3 getRight() const noexcept;
        glm::vec3 getUp() const noexcept;
        glm::vec3 getFront() const noexcept;
        glm::vec3 getPosition() const noexcept;
    
    private :
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::vec3 upWorld;
        float yaw;
        float pitch;
        float moveSpeed;
        float sensitivity;
        float fov;

        void updateCamera() noexcept;
};

#endif