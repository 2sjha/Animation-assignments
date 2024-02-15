#include "Aircraft_Animation.h"
#include "Curve.h"

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
    m_model_mat =
        glm::translate(m_model_mat, m_animation_curve->control_points_pos[0]);

    v0 = 1.0;
    acc = 1.0;
    d1 = 0.0;
    d2 = 0.0;
    normalized_t = -1.0;
  }
}

void Aircraft_Animation::update(float normalized_time,
                                glm::float32 normalized_ease_dist) {
  glm::vec3 point_on_curve;
  int idx = binary_search_curve_dist(normalized_ease_dist);
  if (idx < 10 or idx > m_animation_curve->arc_distances.size() - 10) {
    point_on_curve = m_animation_curve->curve_points_pos[idx];
  } else {

    glm::float32 fraction =
        (normalized_ease_dist - m_animation_curve->arc_distances[idx]) /
        (m_animation_curve->arc_distances[idx + 1] -
         m_animation_curve->arc_distances[idx]);

    glm::vec3 p0 = m_animation_curve->curve_points_pos[idx - 1];
    glm::vec3 p1 = m_animation_curve->curve_points_pos[idx];
    glm::vec3 p2 = m_animation_curve->curve_points_pos[idx + 1];
    glm::vec3 p3 = m_animation_curve->curve_points_pos[idx + 2];

    point_on_curve =
        m_animation_curve->get_point_catmull_rom(p0, p1, p2, p3, fraction);
  }
  glm::vec3 translation_vec = point_on_curve - glm::vec3(m_model_mat[3]);
  m_model_mat = glm::translate(m_model_mat, translation_vec);
}

void Aircraft_Animation::start_moving() {
  this->v0 = m_animation_curve->total_arc_dist / this->total_moving_time;
  this->normalized_t = 0.0;
  this->acc =
      (2 * (m_animation_curve->total_arc_dist -
            v0 * (1 - t1) * total_moving_time)) /
      ((t1 * t1 - (1 - t2) * (1 - t2)) * total_moving_time * total_moving_time);
}

glm::float32 Aircraft_Animation::get_ease_dist(glm::float32 normalized_t) {
  glm::float32 dist = 0.0;

  if (normalized_t == 0.0) {
    dist = 0.0;
  } else if (normalized_t > 0.0 && normalized_t < t1) {
    dist = (0.5 * acc * normalized_t * normalized_t * total_moving_time *
            total_moving_time); // constant acceleration
  } else if (normalized_t >= t1 && normalized_t <= t2) {
    if (d1 == 0.0) {
      d1 = 0.5 * acc * t1 * t1 * total_moving_time * total_moving_time;
    }
    dist = (d1 + v0 * (normalized_t - this->t1) * total_moving_time);
  } else if (normalized_t >= t2 && normalized_t < 1.0) {
    if (d1 == 0.0) {
      d1 = 0.5 * acc * t1 * t1 * total_moving_time * total_moving_time;
    }
    if (d2 == 0.0) {
      d2 = d1 + v0 * (t2 - t1) * total_moving_time;
    }
    dist = (d2 + v0 * (normalized_t - t2) * total_moving_time -
            0.5 * acc * ((normalized_t - t2) * total_moving_time) *
                ((normalized_t - t2) * total_moving_time));
  }

  return dist / m_animation_curve->total_arc_dist;
}

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