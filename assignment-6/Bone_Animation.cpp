#include "Bone_Animation.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>

Bone_Animation::Bone_Animation() {
}

Bone_Animation::~Bone_Animation() {
}

void Bone_Animation::init() {
    root_position = {2.0f, 0.5f, 2.0f};

    scale_vector = {{1.0f, 1.0f, 1.0f},
                    {0.5f, 4.0f, 0.5f},
                    {0.5f, 3.0f, 0.5f},
                    {0.5f, 2.0f, 0.5f}};

    colors = {{0.7f, 0.0f, 0.0f, 1.0f},
              {0.7f, 0.7f, 0.0f, 1.0f},
              {0.7f, 0.0f, 0.7f, 1.0f},
              {0.0f, 0.7f, 0.7f, 1.0f}};

    target_position = {3.0, 8.0, 3.0};
    target_scale_vector = {1.0f, 1.0f, 1.0f};
    target_color = {0.0f, 0.7f, 0.0f, 1.0f};

    reset();
}

void Bone_Animation::update(float delta_time) {
    // Update target cube's matrix if target_position is updated in GUI
    target_mat = glm::translate(glm::mat4(1.0f), target_position);

    // Dont update bones if distance is less than threshold
    // and stop moving
    if (glm::distance(target_position, end_effector_position) <
        target_dist_threshold) {
        is_moving = false;
        return;
    }

    // Dont update bones if checkbox is not clicked
    if (!is_moving) {
        return;
    }

    // Calculate Jacobian
    for (int i = 1; i <= 3; ++i) {
        // Only create x, z rotation matrices since the a'x, a'y, a'z equations
        // in Hints PDF use only 2 rotation matrices
        glm::mat4 rotate_x = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][0]),
            glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotate_z = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][2]),
            glm::vec3(0.0, 0.0, 1.0));

        // Use the parent-world matrix to get a'x, a'y, a'z [equations in the
        // Hints PDF]
        glm::vec3 axis_y =
            world_mat[i - 1] * rotate_x * rotate_z * glm::vec4(0, 1, 0, 0);
        glm::vec3 axis_z = world_mat[i - 1] * rotate_x * glm::vec4(0, 0, 1, 0);
        glm::vec3 axis_x = world_mat[i - 1] * glm::vec4(1, 0, 0, 0);

        // Calculate pivot point r' in world space, pivot r for ith bone is
        // parent's end
        glm::mat4 parent_end_mat =
            glm::translate(glm::mat4(1.0f), {0, scale_vector[i - 1][1], 0});
        glm::vec3 pivot = world_mat[i - 1] * parent_end_mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat3 jacobian =
            glm::mat3(glm::cross(axis_x, (end_effector_position - pivot)),
                      glm::cross(axis_y, (end_effector_position - pivot)),
                      glm::cross(axis_z, (end_effector_position - pivot)));

        glm::mat3 jacobian_transpose = glm::transpose(jacobian);

        // Dot product with self to get ||A||^2
        float step_size =
            (dot(jacobian_transpose * (target_position - end_effector_position),
                 jacobian_transpose *
                     (target_position - end_effector_position))) /
            (dot(jacobian * jacobian_transpose *
                     (target_position - end_effector_position),
                 jacobian * jacobian_transpose *
                     (target_position - end_effector_position)));

        // calculate delta theta
        glm::vec3 rotation_angle_update =
            step_size * jacobian_transpose *
            (target_position - end_effector_position);

        rotation_degree_vector[i] = {
            rotation_degree_vector[i][0] + rotation_angle_update[0],
            rotation_degree_vector[i][1] + rotation_angle_update[1],
            rotation_degree_vector[i][2] + rotation_angle_update[2]
        };
    }

    tranform_bones();
}

void Bone_Animation::reset() {
    std::vector<glm::mat4> init_mat4 = {glm::mat4(1.0f), glm::mat4(1.0f),
                                        glm::mat4(1.0f), glm::mat4(1.0f)};

    //  The rotations in the following order: y-axis, z-axis, x-axis. The
    //  initial pose vector for each bone is (0.0, 30.0, 0.0), with all numbers
    //  in degrees.

    //  I'm using X,Y,Z order in the vector but rotation order is as
    //  mentioned in the pdf
    rotation_degree_vector = {{0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 30.0f},
                              {0.0f, 0.0f, 30.0f},
                              {0.0f, 0.0f, 30.0f}};

    rotation_mat = init_mat4;
    translation_mat = init_mat4;
    world_mat = init_mat4;
    final_mat = init_mat4;
    target_mat = glm::mat4(1.0f);

    is_moving = false;
    setup_links();
    tranform_bones();
}

void Bone_Animation::setup_links() {
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

void Bone_Animation::tranform_bones() {
    for (int i = 1; i <= 3; i++) {
        // Get X,Y,Z Rotation matrices
        glm::mat4 rotate_x = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][0]),
            glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotate_y = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][1]),
            glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 rotate_z = glm::rotate(
            glm::mat4(1.0f), glm::radians(rotation_degree_vector[i][2]),
            glm::vec3(0.0, 0.0, 1.0));

        // Rotation Matrix in the order of rotations mentioned
        // in the project PDF
        rotation_mat[i] = rotate_x * rotate_z * rotate_y;

        // Rotate first, then translate to link position
        // Link position at the y-axis end of the parent model
        glm::mat4 link_mat =
            glm::translate(glm::mat4(1.0f), {0, scale_vector[i - 1][1], 0});
        glm::mat4 local_transformation_mat = link_mat * rotation_mat[i];

        // Update World matrix using parent world matrix
        world_mat[i] = world_mat[i - 1] * local_transformation_mat;

        // Finally, translate to origin, then apply world transformations
        final_mat[i] = world_mat[i] * translation_mat[i];
    }

    // Calculate End effector based on last bone
    glm::mat4 end_mat =
        glm::translate(glm::mat4(1.0f), {0, scale_vector[3][1], 0});
    end_effector_position = world_mat[3] * end_mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
