#include <plum/util/transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Transform::Transform()
    : Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1)) 
{}

Transform::Transform(glm::mat4 matrix)
    : position(glm::vec3(matrix[3])), lastPosition(position),
    rotationQuat(glm::quat(matrix)), lastRotationQuat(rotationQuat),
    rotationEuler(glm::eulerAngles(rotationQuat)), lastRotationEuler(rotationEuler),
    scale(extractScale(matrix)), lastScale(scale)
{
    Update();
}

Transform::Transform(glm::vec3 position, glm::vec3 rotationEuler, glm::vec3 scale)
    : position(position), lastPosition(position),
    rotationQuat(glm::quat(rotationEuler)), lastRotationQuat(rotationQuat),
    rotationEuler(rotationEuler), lastRotationEuler(rotationEuler),
    scale(scale), lastScale(scale)
{
    Update();
}

Transform::Transform(glm::vec3 position, glm::quat rotationQuat, glm::vec3 scale) 
    : position(position), lastPosition(position),
    rotationQuat(rotationQuat), lastRotationQuat(rotationQuat),
    rotationEuler(glm::eulerAngles(rotationQuat)), lastRotationEuler(rotationEuler),
    scale(scale), lastScale(scale)
{
    Update();
}
    
const glm::mat4& Transform::Matrix() {
    if (isUpdateRequired) {
        Update();
        isUpdateRequired = false;
    }
    return matrix;
}

void Transform::Translate(glm::vec3 translation) {
    lastPosition = position;
    position += translation;
    isUpdateRequired = true;
}
void Transform::Translate(float dx, float dy, float dz) {
    lastPosition = position;
    position += glm::vec3(dx, dy, dz);
    isUpdateRequired = true;
}

void Transform::Rotate(glm::vec3 eulerAngles) {
    lastRotationEuler = rotationEuler;
    lastRotationQuat = rotationQuat;
    rotationEuler += eulerAngles;
    rotationQuat = glm::quat(rotationEuler);
    updateFrontRightUp();
    isUpdateRequired = true;
}
void Transform::Rotate(float pitch, float yaw, float roll) {
    lastRotationEuler = rotationEuler;
    lastRotationQuat = rotationQuat;
    rotationEuler += glm::vec3(pitch, yaw, roll);
    rotationQuat = glm::quat(rotationEuler);
    updateFrontRightUp();
    isUpdateRequired = true;
}
void Transform::LookAt(glm::vec3 target, glm::vec3 up) {
    lastRotationEuler = rotationEuler;
    lastRotationQuat = rotationQuat;
    glm::vec3 direction = glm::normalize(target - position);
    rotationQuat = glm::quatLookAt(direction, up);
    rotationEuler = glm::eulerAngles(rotationQuat);
    updateFrontRightUp();
    isUpdateRequired = true;
}

void Transform::Scale(float s) {
    lastScale = scale;
    scale *= s;
    isUpdateRequired = true;
}
void Transform::Scale(glm::vec3 s) {
    lastScale = scale;
    scale = s;
    isUpdateRequired = true;
}
void Transform::Scale(float xscale, float yscale, float zscale) {
    lastScale = scale;
    scale = glm::vec3(xscale, yscale, zscale);
    isUpdateRequired = true;
}

void Transform::Update() {
    if (position != lastPosition) {
        matrix[3] = glm::vec4(position, 1);
        lastPosition = position;
    }
    if (rotationQuat != lastRotationQuat || rotationEuler != lastRotationEuler || scale != lastScale) {
        if (rotationQuat != lastRotationQuat) {
            glm::mat3 rotationMatrix = glm::mat3(rotationQuat);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    matrix[i][j] = rotationMatrix[i][j];
            updateFrontRightUp();
            lastRotationQuat = rotationQuat;
        }
        if (rotationEuler != lastRotationEuler) {
            glm::mat4 rotationMatrix = glm::yawPitchRoll(rotationEuler.x, rotationEuler.y, rotationEuler.z);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    matrix[i][j] = rotationMatrix[i][j];
            updateFrontRightUp();
            lastRotationEuler = rotationEuler;
            rotationQuat = glm::quat(rotationEuler);
            lastRotationQuat = rotationQuat;
        }
        matrix = glm::scale(matrix, scale/lastScale);
        lastScale = scale;
    }
}

void Transform::updateFrontRightUp() {
    glm::mat3 rotationMatrix = glm::mat3(rotationQuat);
    front = rotationMatrix * glm::vec3(0,0,1);
    right = rotationMatrix * glm::vec3(1,0,0);
    up    = rotationMatrix * glm::vec3(0,1,0);
}

glm::vec3 Transform::extractScale(glm::mat4 matrix) {
    float sx = glm::vec3(matrix[0]).length();
    float sy = glm::vec3(matrix[1]).length();
    float sz = glm::vec3(matrix[2]).length();
    return glm::vec3(sx, sy, sz);
}