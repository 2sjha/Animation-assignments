#include "Animation.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

Animation::Animation() { this->m_model_mat = glm::mat4(1.0f); }

Animation::~Animation() {}

void Animation::init() { reset(); }

void Animation::update(float delta_time) {}

void Animation::reset() {
  m_model_mat = glm::mat4(1.0f);
  m_model_mat = glm::translate(m_model_mat, glm::vec3(5.0f, 0.0f, 0.0f));

  local_angle_a = 0.0f;
  total_angle_a = 0.0f;
  global_angle_b = 0.0f;
}

void Animation::rotate_local_x() {
  // Maintain total angle that can be used to do reverse rotation 
  total_angle_a = std::fmod(total_angle_a + local_angle_a, 360.0f);

  // x rotation around self axis
  m_model_mat = glm::rotate(m_model_mat, glm::radians(local_angle_a),
                            glm::vec3(1.0f, 0.0f, 0.0f));
}

void Animation::rotate_global_y() {
  // reverse total x rotation around self x axis
  m_model_mat = glm::rotate(m_model_mat, glm::radians(-total_angle_a),
                            glm::vec3(1.0f, 0.0f, 0.0f));

  // translate to center
  m_model_mat = glm::translate(m_model_mat, glm::vec3(-5.0f, 0.0f, 0.0f));
  // rotate around self y axis
  m_model_mat = glm::rotate(m_model_mat, glm::radians(global_angle_b),
                            glm::vec3(0.0f, 1.0f, 0.0f));
  // translate to original posn
  m_model_mat = glm::translate(m_model_mat, glm::vec3(5.0f, 0.0f, 0.0f));

  // x rotation to original angle
  m_model_mat = glm::rotate(m_model_mat, glm::radians(total_angle_a),
                            glm::vec3(1.0f, 0.0f, 0.0f));
}