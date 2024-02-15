#pragma once
#include <iostream>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Curve {
private:
  void reset_curve();
  void catmull_rom_curve();

public:
  Curve();
  ~Curve();

  void init();
  void calculate_curve();
  glm::vec3 get_point_catmull_rom(glm::vec3 p_0, glm::vec3 p_1, glm::vec3 p_2,
                                  glm::vec3 p_3, float u);

public:
  float tau = 0.5; // Coefficient for catmull-rom spline
  int num_points_per_segment = 200;
  glm::float32 total_arc_dist = 0.0;
  bool enabled = false;

  std::vector<glm::vec3> control_points_pos;
  std::vector<glm::quat> control_points_quaternion;
  std::vector<glm::vec3> curve_points_pos;
  std::vector<glm::float32> arc_distances;
  // contains arcdistances until each control point
  std::vector<glm::float32> control_arc_distances;
};