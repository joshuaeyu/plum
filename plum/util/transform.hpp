#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
    public:
        Transform();
        
        glm::mat4 Matrix();
        
        void Rotate(glm::quat rotation);
        void Rotate(float pitch, float yaw, float roll);
        void LookAt(glm::vec3 target);
        void Translate(glm::vec3 translation);
        void Translate(float dx, float dy, float dz);
        void Scale(float scale);
        void Scale(float xscale, float yscale, float zscale);

        glm::vec3 position = glm::vec3(0,0,0);
        glm::vec3 rotationEuler = glm::vec3(0,0,0);
        glm::vec3 scale = glm::vec3(1,1,1);
        
    protected:
    
    private:
        glm::mat4 matrix = glm::identity<glm::mat4>();
        glm::quat rotationQuat = glm::quat(glm::vec3(0,0,0));

};