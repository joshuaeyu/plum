#include <plum/util/transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Transform::Transform()
    : Transform(glm::vec3(0), glm::vec3(0), glm::vec3(1)) 
{}

Transform::Transform(glm::mat4 matrix)
    : position(glm::vec3(matrix[3])), lastPosition(position),
    rotationQuat(glm::quat(matrix)), lastRotationQuat(rotationQuat),
    scale(ExtractScale(matrix)), lastScale(scale)
{
    Update();
    updateFrontRightUp();
}

Transform::Transform(glm::vec3 position, glm::vec3 rotationEuler, glm::vec3 scale)
    : position(position), lastPosition(position),
    rotationQuat(glm::quat(glm::radians(rotationEuler))), lastRotationQuat(rotationQuat),
    // rotationEuler(rotationEuler), lastRotationEuler(rotationEuler),
    scale(scale), lastScale(scale)
{
    Update();
    updateFrontRightUp();
}

Transform::Transform(glm::vec3 position, glm::quat rotationQuat, glm::vec3 scale) 
    : position(position), lastPosition(position),
    rotationQuat(rotationQuat), lastRotationQuat(rotationQuat),
    // rotationEuler(glm::degrees(glm::eulerAngles(rotationQuat))), lastRotationEuler(rotationEuler),
    scale(scale), lastScale(scale)
{
    Update();
    updateFrontRightUp();
}
    
const glm::mat4& Transform::Matrix() {
    if (isUpdateRequired) {
        Update();
        isUpdateRequired = false;
    }
    return matrix;
}

glm::vec3 Transform::EulerAngles() const {
    return glm::eulerAngles(rotationQuat);
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
    lastRotationQuat = rotationQuat;
    rotationQuat *= glm::quat(glm::radians(eulerAngles));
    updateFrontRightUp();
    isUpdateRequired = true;
}
void Transform::Rotate(float pitch, float yaw, float roll) {
    Rotate(glm::vec3(pitch, yaw, roll));
}
void Transform::LookAt(glm::vec3 target, glm::vec3 up) {
    lastRotationQuat = rotationQuat;
    glm::vec3 direction = glm::normalize(target - position);
    rotationQuat = glm::quatLookAt(direction, up);
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
    if (rotationQuat != lastRotationQuat || scale != lastScale) {
        if (rotationQuat != lastRotationQuat) {
            glm::mat3 rotationMatrix = glm::mat3(rotationQuat);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    matrix[i][j] = rotationMatrix[i][j];
            updateFrontRightUp();
            lastRotationQuat = rotationQuat;
        }
        matrix = glm::scale(matrix, scale/lastScale);
        if (scale != lastScale) {
            lastScale = scale;
        }
    }
}

void Transform::updateFrontRightUp() {
    front = rotationQuat * glm::vec3(0,0,1);
    right = rotationQuat * glm::vec3(1,0,0);
    up    = rotationQuat * glm::vec3(0,1,0);
}

glm::vec3 Transform::ExtractScale(const glm::mat4 matrix) {
    float sx = glm::length(glm::vec3(matrix[0]));
    float sy = glm::length(glm::vec3(matrix[1]));
    float sz = glm::length(glm::vec3(matrix[2]));
    return glm::vec3(sx, sy, sz);
}

glm::mat4 Transform::ExtractRotation(const glm::mat4 matrix) {
    glm::vec3 scale = ExtractScale(matrix);
    glm::mat4 result = matrix;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result[i][j] /= scale[i];
    result[3] = glm::vec4(0,0,0,1);
    return result;
}