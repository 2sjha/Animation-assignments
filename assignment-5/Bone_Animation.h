#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Bone_Animation {
    public:
        Bone_Animation();
        ~Bone_Animation();

        void init();
        void update(float delta_time);
        void reset();
		void setup_linkages();

    private:
        std::vector<glm::mat4> rotation_mat;
        std::vector<glm::mat4> translation_mat;
        std::vector<glm::mat4> world_mat;

    public:
        // Here the head of each vector is the root bone
        std::vector<glm::vec3> scale_vector;
        std::vector<glm::vec3> rotation_degree_vector;
        std::vector<glm::vec4> colors;

        glm::vec3 root_position;

        std::vector<glm::mat4> final_mat;
};
