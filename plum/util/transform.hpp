#pragma once

#include <plum/util/direction.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
    public:
        Transform();
        Transform(const glm::mat4& matrix);
        Transform(const glm::vec3& position, const glm::vec3& rotation_euler, const glm::vec3& scale);
        Transform(const glm::vec3& position, const glm::quat& rotation_quat, const glm::vec3& scale);
        
        glm::vec3 position;      // If changing directly, must call Update() to update matrix
        glm::vec3 rotationEuler; // If changing directly, must call Update() to update matrix
        glm::quat rotationQuat;  // If changing directly, must call Update() to update matrix
        glm::vec3 scale;         // If changing directly, must call Update() to update matrix
        
        const glm::mat4& Matrix();
        const glm::vec3& Front() const { return front; }
        const glm::vec3& Right() const { return right; }
        const glm::vec3& Up() const { return up; }
        glm::vec3 EulerAngles() const { return glm::degrees(rotationEuler); }
        
        void Translate(const glm::vec3& translation);
        void Translate(float dx, float dy, float dz);
        void Rotate(const glm::vec3& euler_angles);
        void Rotate(float pitch, float yaw, float roll);
        void LookAt(const glm::vec3& target, const glm::vec3& up = Direction::up);
        void Scale(float scale);
        void Scale(const glm::vec3& scale);
        void Scale(float xscale, float yscale, float zscale);
        
        
        void Update(); // Update matrix, front, up, and right based on position, rotationEuler (prioritized over rotationQuat), and scale
        
        // Utility functions
        static glm::vec3 ExtractScale(const glm::mat4& matrix);
        static glm::mat4 ExtractRotation(const glm::mat4& matrix);
        
    private:
        glm::mat4 matrix = glm::identity<glm::mat4>();
        glm::vec3 front = Direction::front;
        glm::vec3 right = Direction::right;
        glm::vec3 up = Direction::up;
        
        bool isUpdateRequired = false;
        glm::vec3 lastPosition = glm::vec3(0,0,0);
        glm::vec3 lastRotationEuler = glm::vec3(0,0,0);
        glm::quat lastRotationQuat = glm::identity<glm::quat>();
        glm::vec3 lastScale = glm::vec3(1,1,1);
        
        void updateFrontRightUp();
};