#include "Aircraft_Animation.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

Aircraft_Animation::Aircraft_Animation() {
  this->m_model_mat = glm::mat4(1.0f);
}

Aircraft_Animation::~Aircraft_Animation() {}

void Aircraft_Animation::init() {
  this->v0 = 0.0;
  reset();
}

void Aircraft_Animation::init(Curve *animation_curve) {
  m_animation_curve = animation_curve;
  reset();
}

void Aircraft_Animation::update(float delta_time) {
  // Do nothing if normalized_t not set
  if (normalized_t == -1.0) {
    return;
  } else {
    normalized_t += delta_time / total_moving_time;
    if (normalized_t > 1.0) {
      reset();
    } else {
      update(normalized_t, get_ease_dist(normalized_t));
    }
  }
}

void Aircraft_Animation::reset() {
  m_model_mat = glm::mat4(1.0f);
  if (m_animation_curve != nullptr &&
      m_animation_curve->control_points_pos.size() > 0) {
    // Translate & Rotate Aircraft to first control point
    m_model_mat =
        m_model_mat *
        get_transformation_mat(m_animation_curve->control_points_pos[0],
                               m_animation_curve->control_points_quaternion[0]);

    v0 = 1.0;
    acc = 1.0;
    d1 = 0.0;
    d2 = 0.0;
    normalized_t = -1.0;
  }
}

/**
For @param normalized_time and @param normalized_ease_dist
binary searches on the arc-distances array for an appropriate point on curve,
then exactly interpolates between that point and the next point on curve.
Also, calcualtes orientation quaternion along the curve based on the distance
travelled along the curve.

Finally, transforms the model to that point and orientation
*/
void Aircraft_Animation::update(float normalized_time,
                                glm::float32 normalized_ease_dist) {
  glm::vec3 p0, p1, p2, p3, point_on_curve;
  glm::quat q1, q2, rotation_q;
  glm::mat4 rotation_mat;
  glm::float32 dist_fraction, rotation_fraction;

  // Interpolating trnslation
  int idx = binary_search_curve_dist(normalized_ease_dist);
  if (idx < 3 or idx > m_animation_curve->arc_distances.size() - 3) {
    point_on_curve = m_animation_curve->curve_points_pos[idx];
  } else {
    dist_fraction =
        (normalized_ease_dist - m_animation_curve->arc_distances[idx]) /
        (m_animation_curve->arc_distances[idx + 1] -
         m_animation_curve->arc_distances[idx]);
    p0 = m_animation_curve->curve_points_pos[idx - 1];
    p1 = m_animation_curve->curve_points_pos[idx];
    p2 = m_animation_curve->curve_points_pos[idx + 1];
    p3 = m_animation_curve->curve_points_pos[idx + 2];
    point_on_curve =
        m_animation_curve->get_point_catmull_rom(p0, p1, p2, p3, dist_fraction);
  }

  // Interpolating rotation
  int m = m_animation_curve->num_points_per_segment + 1;
  int n = m_animation_curve->control_arc_distances.size();
  int control_point_idx = idx / m;
  if (control_point_idx == 0) {
    rotation_fraction =
        normalized_ease_dist / m_animation_curve->control_arc_distances[0];
  } else {
    rotation_fraction =
        (normalized_ease_dist -
         m_animation_curve->control_arc_distances[control_point_idx - 1]) /
        (m_animation_curve->control_arc_distances[control_point_idx] -
         m_animation_curve->control_arc_distances[control_point_idx - 1]);
  }

  // Interpolated rotation quaternion
  q1 = m_animation_curve->control_points_quaternion[control_point_idx];
  q2 =
      m_animation_curve->control_points_quaternion[(control_point_idx + 1) % n];
  rotation_q = glm::slerp(q1, q2, rotation_fraction);

  m_model_mat =
      get_transformation_mat(point_on_curve, rotation_q) * glm::mat4(1.0f);
}

/**
Initiates the movement of the aircraft
*/
void Aircraft_Animation::start_moving() {
  this->v0 = m_animation_curve->total_arc_dist / this->total_moving_time;
  this->normalized_t = 0.0;

  // Need to calculate acceleration, since we know total_t, total_dist, t1, t2
  // and we've assumed a v0 = total_dist/total_t, only unknown is acc.
  this->acc =
      (2 * (m_animation_curve->total_arc_dist -
            v0 * (1 - t1) * total_moving_time)) /
      ((t1 * t1 - (1 - t2) * (1 - t2)) * total_moving_time * total_moving_time);
}

/**
For @param normalized_t time from 0.0 t0 0.1, returns normalized distance
based on ease_in and ease_out constant acceleration formula
*/
glm::float32 Aircraft_Animation::get_ease_dist(glm::float32 normalized_t) {
  glm::float32 dist = 0.0;
  glm::float32 curr_time = normalized_t * total_moving_time;
  glm::float32 t1_time = t1 * total_moving_time;
  glm::float32 t2_time = t2 * total_moving_time;

  if (normalized_t == 0.0) {
    dist = 0.0;
  } else if (normalized_t > 0.0 && normalized_t < t1) {
    dist = (0.5 * acc * curr_time * curr_time); // constant acceleration
  } else if (normalized_t >= t1 && normalized_t < t2) {
    if (d1 == 0.0) {
      d1 = 0.5 * acc * t1_time * t1_time;
    }
    dist = (d1 + v0 * (curr_time - t1_time)); // constant speed
  } else if (normalized_t >= t2 && normalized_t < 1.0) {
    if (d1 == 0.0) {
      d1 = 0.5 * acc * t1_time * t1_time;
    }
    if (d2 == 0.0) {
      d2 = d1 + v0 * (t2_time - t1_time);
    }
    dist = (d2 + v0 * (curr_time - t2_time) -
            0.5 * acc * (curr_time - t2_time) * (curr_time - t2_time));
  }

  return dist / m_animation_curve->total_arc_dist;
}

/**
For @param normalized_dist binary search for an index on arc_distances
vector, which is smaller than normalized_dist but next index is bigger than
normalized_dist
*/
int Aircraft_Animation::binary_search_curve_dist(glm::float32 normalized_dist) {
  int n = this->m_animation_curve->arc_distances.size();
  if (normalized_dist == 0.0)
    return 0;
  else if (normalized_dist == 1.0)
    return n - 1;

  int low = 0;
  int high = n - 1;
  int mid;

  while (low < high) {
    mid = low + (high - low) / 2;
    if (this->m_animation_curve->arc_distances[mid] <= normalized_dist &&
        mid < n - 1 &&
        this->m_animation_curve->arc_distances[mid + 1] > normalized_dist) {
      return mid;
    } else if (this->m_animation_curve->arc_distances[mid] < normalized_dist &&
               mid < n - 1 &&
               this->m_animation_curve->arc_distances[mid + 1] <
                   normalized_dist) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return mid;
}

/**
Creates final trnsformation matrix based off @param quat quaternion and a @param
pos position. Based off of the formula in the assignment pdf
*/
glm::mat4 Aircraft_Animation::get_transformation_mat(glm::vec3 pos,
                                                     glm::quat quat) {
  glm::mat4 res_mat = glm::mat4(1.0f);
  glm::float32 x, y, z, s;
  x = quat[0];
  y = quat[1];
  z = quat[2];
  s = quat[3];

  res_mat[0][0] = 1.0 - 2 * y * y - 2 * z * z;
  res_mat[1][0] = 2 * x * y - 2 * s * z;
  res_mat[2][0] = 2 * x * z + 2 * s * y;
  res_mat[3][0] = pos[0];

  res_mat[0][1] = 2 * x * y + 2 * s * z;
  res_mat[1][1] = 1.0 - 2 * x * x - 2 * z * z;
  res_mat[2][1] = 2 * y * z - 2 * s * x;
  res_mat[3][1] = pos[1];

  res_mat[0][2] = 2 * x * z - 2 * s * y;
  res_mat[1][2] = 2 * y * z + 2 * s * x;
  res_mat[2][2] = 1.0 - 2 * x * x - 2 * y * y;
  res_mat[3][2] = pos[2];

  res_mat[0][3] = 0;
  res_mat[1][3] = 0;
  res_mat[2][3] = 0;
  res_mat[3][3] = 1;

  return res_mat;
}