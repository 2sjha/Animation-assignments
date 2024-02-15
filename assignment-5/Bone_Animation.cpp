#include "Bone_Animation.h"

Bone_Animation::Bone_Animation() {
}

Bone_Animation::~Bone_Animation() {
}

void Bone_Animation::init() {
    root_position = {2.0f, 1.0f, 2.0f};

    scale_vector = {{1.0f, 1.0f, 1.0f},
                    {0.5f, 4.0f, 0.5f},
                    {0.5f, 3.0f, 0.5f},
                    {0.5f, 2.0f, 0.5f}};

    colors = {{0.7f, 0.0f, 0.0f, 1.0f},
              {0.7f, 0.7f, 0.0f, 1.0f},
              {0.7f, 0.0f, 0.7f, 1.0f},
              {0.0f, 0.7f, 0.7f, 1.0f}};

    reset();
}

void Bone_Animation::update(float delta_time) {
    for (int i = 1; i <= 3; i++) {
        // Get X,Y,Z Rotation matrices from X,Y,Z Euler angles
        glm::mat4 rotateX = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][0]),
            glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotateY = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][1]),
            glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 rotateZ = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][2]),
            glm::vec3(0.0, 0.0, 1.0));

        // Rotation Matrix in the order of rotations mentioned
        // in the project PDF
        rotation_mat[i] = rotateX * rotateZ * rotateY;

        // Rotate first, then translate to link position
        // Link position at the y-axis end of the parent/previous model
        glm::mat4 link_mat =
            glm::translate(glm::mat4(1.0f), {0, scale_vector[i-1][1], 0});
        glm::mat4 local_transformation_mat = link_mat * rotation_mat[i];

        // Update World matrix using parent/previous world matrix
        world_mat[i] = world_mat[i - 1] * local_transformation_mat;

		// Finally, translate to origin, then apply world transformations
        final_mat[i] = world_mat[i] * translation_mat[i];
    }
}

void Bone_Animation::reset() {
    std::vector<glm::mat4> init_mat4 = {glm::mat4(1.0f), glm::mat4(1.0f),
                                        glm::mat4(1.0f), glm::mat4(1.0f)};

    rotation_degree_vector = {{0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f}};

    rotation_mat = init_mat4;
    translation_mat = init_mat4;
    world_mat = init_mat4;
    final_mat = init_mat4;

    setup_linkages();
}

void Bone_Animation::setup_linkages() {
    // Setup world matrix for root bone such that
    // it can be used for children nodes
    glm::vec3 world_origin = {0, scale_vector[0][1] / 2, 0};
    world_mat[0] =
        glm::translate(glm::mat4(1.0f), root_position - world_origin);

    // Translate root bone to root position
    final_mat[0] = glm::translate(glm::mat4(1.0f), root_position);

    // Translate origin for children nodes
    for (int i = 1; i <= 3; ++i) {
        // Translate origin to center of y co-ordinate of models
        translation_mat[i] =
            glm::translate(glm::mat4(1.0f), {0, scale_vector[i][1] / 2, 0});
    }
}
