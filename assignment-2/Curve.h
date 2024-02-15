#pragma once
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Curve {
private:
  void reset_curve();
  void catmull_rom_curve();
  glm::vec3 get_point_catmull_rom(glm::vec3 p_0, glm::vec3 p_1, glm::vec3 p_2,
                                  glm::vec3 p_3, float u);

public:
  Curve();
  ~Curve();

  void init();
  void calculate_curve();

public:
  float tau = 0.5; // Coefficient for catmull-rom spline
  int num_points_per_segment = 200;
  bool enabled = false;

  std::vector<glm::vec3> control_points_pos;
  std::vector<glm::vec3> curve_points_pos;
};