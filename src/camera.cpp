#include <camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera( glm::vec3 position, glm::vec3 up, float yaw, float pitch, float fov, float sensitivity, float moveSpeed ) noexcept :
    position( position ), upWorld( up ), yaw( yaw ), pitch( pitch ), fov( fov ), sensitivity( sensitivity ), moveSpeed( moveSpeed )
{
    updateCamera();
}

void Camera::onKeyboard( CameraMovement direction, float deltaTime ) noexcept
{
    float velocity = moveSpeed * deltaTime;
    float ogHeight = position.y;
    switch( direction )
    {
        case FORWARD : position += front * velocity; position.y = ogHeight; break;
        case RIGHT : position += right * velocity; position.y = ogHeight; break;
        case BACKWARD : position -= front * velocity; position.y = ogHeight; break;
        case LEFT : position -= right * velocity; position.y = ogHeight; break;
        case UP : position += up * velocity; break;
        case DOWN : position -= up * velocity; break;
    }
}

void Camera::onMouseMove( float xOffset, float yOffset, bool constrainPitch ) noexcept
{
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;
    if( constrainPitch )
    {
        if( pitch > 89.0f )
            pitch = 89.0f;
        if( pitch < -89.0f )
            pitch = -89.0f;
    }
    updateCamera();
}

void Camera::onScroll( float offset ) noexcept
{
    fov -= offset;
    if( fov < 1.0f )
        fov = 1.0f;
    if( fov > 89.0f )
        fov = 89.0f;
}

void Camera::updateCamera() noexcept
{
    glm::vec3 newFront;
    newFront.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    newFront.y = sin( glm::radians( pitch ) );
    newFront.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
    front = glm::normalize( newFront );
    right = glm::normalize( glm::cross( front, upWorld ) );
    up = glm::normalize( glm::cross( right, front ) );
}

glm::mat4 Camera::getViewMat() const noexcept
{
    return glm::lookAt( position, position + front, up );
}

float Camera::getFov() const noexcept
{
    return fov;
}

float Camera::getSensitivity() const noexcept
{
    return sensitivity;
}

float Camera::getMoveSpeed() const noexcept
{
    return moveSpeed;
}

float Camera::getPitch() const noexcept
{
    return pitch;
}

float Camera::getYaw() const noexcept
{
    return yaw;
}

glm::vec3 Camera::getUpWorld() const noexcept
{
    return upWorld;
}

glm::vec3 Camera::getRight() const noexcept
{
    return right;
}

glm::vec3 Camera::getFront() const noexcept
{
    return front;
}

glm::vec3 Camera::getUp() const noexcept
{
    return up;
}

glm::vec3 Camera::getPosition() const noexcept
{
    return position;
}