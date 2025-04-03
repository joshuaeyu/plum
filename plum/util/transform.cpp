#include <plum/util/transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Transform::Transform()
    : Transform(glm::identity<glm::mat4>()) 
{}

Transform::Transform(const glm::mat4& matrix)
    : matrix(matrix),
    position(matrix[3]),
    rotationQuat(matrix),
    rotationEuler(glm::degrees(glm::eulerAngles(rotationQuat))),
    scale(ExtractScale(matrix))
{
    updateFrontRightUp();
}

Transform::Transform(const glm::vec3& position, const glm::vec3& rotation_euler, const glm::vec3& scale)
    : position(position),
    rotationEuler(rotation_euler),
    rotationQuat(glm::radians(rotation_euler)),
    scale(scale)
{
    Update();
}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation_quat, const glm::vec3& scale) 
    : position(position),
    rotationQuat(rotation_quat),
    rotationEuler(glm::degrees(glm::eulerAngles(rotation_quat))),
    scale(scale)
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

void Transform::Translate(const glm::vec3& translation) {
    lastPosition = position;
    position += translation;
    isUpdateRequired = true;
}
void Transform::Translate(float dx, float dy, float dz) {
    lastPosition = position;
    position += glm::vec3(dx, dy, dz);
    isUpdateRequired = true;
}

void Transform::Rotate(const glm::vec3& euler_angles) {
    lastRotationEuler = rotationEuler;
    rotationEuler += euler_angles;
    lastRotationQuat = rotationQuat;
    rotationQuat *= glm::quat(glm::radians(euler_angles));
    updateFrontRightUp();
    isUpdateRequired = true;
}
void Transform::Rotate(float pitch, float yaw, float roll) {
    Rotate(glm::vec3(pitch, yaw, roll));
}
void Transform::LookAt(const glm::vec3& target, const glm::vec3& up) {
    lastRotationQuat = rotationQuat;
    glm::vec3 direction = glm::normalize(target - position);
    rotationQuat = glm::quatLookAt(direction, up);
    lastRotationEuler = rotationEuler;
    rotationEuler = glm::eulerAngles(rotationQuat);
    updateFrontRightUp();
    isUpdateRequired = true;
}

void Transform::Scale(float s) {
    lastScale = scale;
    scale *= s;
    isUpdateRequired = true;
}
void Transform::Scale(const glm::vec3& s) {
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
            updateFrontRightUp();
            glm::mat3 rotationMatrix = glm::mat3(rotationQuat);
            for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            matrix[i][j] = rotationMatrix[i][j];
            matrix = glm::scale(matrix, glm::vec3(scale));
            lastRotationQuat = rotationQuat;
            rotationEuler = glm::degrees(glm::eulerAngles(rotationQuat));
            lastRotationEuler = rotationEuler;
        }
        if (rotationEuler != lastRotationEuler) {
            rotationQuat = glm::quat(glm::radians(rotationEuler));
            updateFrontRightUp();
            glm::mat3 rotationMatrix = glm::mat3(rotationQuat);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    matrix[i][j] = rotationMatrix[i][j];
            matrix = glm::scale(matrix, glm::vec3(scale));
            lastRotationQuat = rotationQuat;
            lastRotationEuler = rotationEuler;
        }
        if (scale != lastScale) {
            scale = glm::clamp(scale, glm::vec3(1e-6f), glm::vec3(1e6f));
            glm::vec3 ratio = glm::dvec3(scale)/glm::dvec3(lastScale);
            matrix = glm::scale(matrix, ratio);
            lastScale = scale;
        }
    }
}

void Transform::updateFrontRightUp() {
    front = rotationQuat * glm::vec3(0,0,1);
    right = rotationQuat * glm::vec3(1,0,0);
    up    = rotationQuat * glm::vec3(0,1,0);
}

glm::vec3 Transform::ExtractScale(const glm::mat4& matrix) {
    float sx = glm::length(glm::vec3(matrix[0]));
    float sy = glm::length(glm::vec3(matrix[1]));
    float sz = glm::length(glm::vec3(matrix[2]));
    return glm::vec3(sx, sy, sz);
}

glm::mat4 Transform::ExtractRotation(const glm::mat4& matrix) {
    glm::vec3 scale = ExtractScale(matrix);
    glm::mat4 result = matrix;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result[i][j] /= scale[i];
    result[3] = glm::vec4(0,0,0,1);
    return result;
}