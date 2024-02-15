#pragma once

#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Curve.h"

class Aircraft_Animation {

public:
  float total_moving_time = 10;
  float t1 = 0.1;
  float t2 = 0.7;
  float v0 = 1.0;
  float acc = 1.0;
  float d1 = 0.0;
  float d2 = 0.0;
  float normalized_t = -1.0;

private:
  glm::mat4 m_model_mat;
  Curve *m_animation_curve = nullptr;

public:
  Aircraft_Animation();
  ~Aircraft_Animation();

  void init();
  void init(Curve *animation_curve);

  void update(float delta_time);
  void update(float delta_time, glm::float32 ease_dist);

  void reset();
  glm::mat4 get_model_mat() { return m_model_mat; };

  void start_moving();
  glm::float32 get_ease_dist(glm::float32 normalized_t);
  int binary_search_curve_dist(glm::float32 normalized_dist);
  glm::mat4 get_transformation_mat(glm::vec3 pos, glm::quat quat);
};
