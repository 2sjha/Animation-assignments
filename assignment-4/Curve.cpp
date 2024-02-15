#include "Curve.h"
#include <glm/ext/quaternion_common.hpp>
#include <string>
#include <vector>

Curve::Curve() {}

Curve::~Curve() {}

void Curve::init() {
  this->control_points_pos = {
      {0.0, 8.5, -2.0}, {-3.0, 11.0, 2.3}, {-6.0, 8.5, -2.5}, {-4.0, 5.5, 2.8},
      {1.0, 2.0, -4.0}, {4.0, 2.0, 3.0},   {7.0, 8.0, -2.0},  {3.0, 10.0, 3.7}};

  this->control_points_quaternion = {
      {0.13964, 0.0481732, 0.831429, 0.541043},
      {0.0509038, -0.033869, -0.579695, 0.811295},
      {-0.502889, -0.366766, 0.493961, 0.592445},
      {-0.636, 0.667177, -0.175206, 0.198922},
      {0.693492, 0.688833, -0.152595, -0.108237},
      {0.752155, -0.519591, -0.316988, 0.168866},
      {0.542054, 0.382705, 0.378416, 0.646269},
      {0.00417342, -0.0208652, -0.584026, 0.810619}};
  calculate_curve();
}

void Curve::reset_curve() {
  this->curve_points_pos = this->control_points_pos;
  this->arc_distances = {};
}

void Curve::calculate_curve() {
  if (this->enabled) {
    catmull_rom_curve();
  } else {
    reset_curve();
  }
}

void Curve::catmull_rom_curve() {
  this->curve_points_pos = std::vector<glm::vec3>();
  int n = this->control_points_pos.size();
  float increment = 1.0 / num_points_per_segment;

  glm::vec3 p0, p1, p2, p3, p;
  glm::vec3 previous = this->control_points_pos[0];
  glm::float32 dist = 0.0;
  for (int i = 0; i < n; ++i) {
    p0 = this->control_points_pos[(i + n - 1) % n];
    p1 = this->control_points_pos[i];
    p2 = this->control_points_pos[(i + 1) % n];
    p3 = this->control_points_pos[(i + 2) % n];

    // 200 increments (1/200 = 0.005)
    for (float j = 0.0; j <= 1.0; j += increment) {
      p = get_point_catmull_rom(p0, p1, p2, p3, j);
      this->curve_points_pos.push_back(p);

      dist += glm::distance(previous, p);
      this->arc_distances.push_back(dist);
      previous = p;
    }
    this->control_arc_distances.push_back(dist);
  }

  // Normalize Arc lengths
  for (int j = 0; j < this->arc_distances.size(); ++j) {
    this->arc_distances[j] /= dist;
  }
  for (int j = 0; j < this->control_arc_distances.size(); ++j) {
    this->control_arc_distances[j] /= dist;
  }
  this->total_arc_dist = dist;
}

/**
 * Create c0, c1, c2, c3 coefficients for the cubic equation
 * p(s) = c0 + c1*u + c2*u^2 + c3*u^3
 * Citation: https://www.cs.cmu.edu/~fp/courses/graphics/asst5/catmullRom.pdf
 */
glm::vec4 get_catmull_rom_coeffs(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,
                                 glm::vec3 p3, float tau, int idx) {
  float c0 = p1[idx];
  float c1 = -1 * tau * p0[idx] + tau * p2[idx];
  float c2 = 2 * tau * p0[idx] + (tau - 3) * p1[idx] + (3 - 2 * tau) * p2[idx] +
             -1 * tau * p3[idx];
  float c3 = -1 * tau * p0[idx] + (2 - tau) * p1[idx] + (tau - 2) * p2[idx] +
             tau * p3[idx];

  return glm::vec4(c0, c1, c2, c3);
}

/**
 * Generates point using Catmull-Rom spline for 0 <= @param u <= 1
 * between @param p1 and @param p2. It uses @param p0 and @param p3 as
 * tangential control points.
 */
glm::vec3 Curve::get_point_catmull_rom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,
                                       glm::vec3 p3, float u) {
  float tau = this->tau;
  glm::vec4 catmull_rom_coeff;
  glm::vec3 p;

  // Get p_x, p_y, p_z
  for (int i = 0; i < 3; ++i) {
    // Get c0, c1, c2, c3 using p0, p1, p2, p3 co-ordinates x or y or z
    catmull_rom_coeff = get_catmull_rom_coeffs(p0, p1, p2, p3, tau, i);

    // p_x,y,z = c0 + c1*u + c2*u^2 + c3*u^3
    p[i] = catmull_rom_coeff[0] + catmull_rom_coeff[1] * u +
           catmull_rom_coeff[2] * u * u + catmull_rom_coeff[3] * u * u * u;
  }

  return p;
}