#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
    public:
        Transform();
        Transform(glm::mat4 matrix);
        Transform(glm::vec3 position, glm::vec3 rotationEuler, glm::vec3 scale);    // Expects euler angles in radians.
        Transform(glm::vec3 position, glm::quat rotationQuat, glm::vec3 scale);

        const glm::mat4& Matrix();
        
        const glm::vec3& Front() const { return front; }
        const glm::vec3& Right() const { return right; }
        const glm::vec3& Up() const { return up; }
        
        void Translate(glm::vec3 translation);
        void Translate(float dx, float dy, float dz);
        void Rotate(glm::vec3 eulerAngles);
        void Rotate(float pitch, float yaw, float roll);
        void LookAt(glm::vec3 target, glm::vec3 up = glm::vec3(0,1,0));
        void Scale(float scale);
        void Scale(glm::vec3 scale);
        void Scale(float xscale, float yscale, float zscale);
        
        // Update matrix, front, up, and right based on position, rotationEuler (prioritized over rotationQuat), and scale
        void Update();  
        glm::vec3 position;      // If changing directly, must call Update() to update matrix
        glm::vec3 rotationEuler; // If changing directly, must call Update() to update matrix
        glm::quat rotationQuat;  // If changing directly, must call Update() to update matrix
        glm::vec3 scale;         // If changing directly, must call Update() to update matrix
        
    private:
        glm::mat4 matrix = glm::identity<glm::mat4>();
        
        bool isUpdateRequired = false;
        glm::vec3 lastPosition;
        glm::quat lastRotationQuat;
        glm::vec3 lastRotationEuler;
        glm::vec3 lastScale;

        void updateFrontRightUp();
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;
        
        static glm::vec3 extractScale(glm::mat4 matrix);

};