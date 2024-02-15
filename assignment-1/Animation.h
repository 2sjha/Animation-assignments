#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

class Animation
{
public:

	GLfloat local_angle_a;
	GLfloat total_angle_a;
	GLfloat global_angle_b;

	Animation();
	~Animation();

	void init();
	void update(float delta_time);

	void reset();
	glm::mat4 get_model_mat() { return m_model_mat; };

	void rotate_local_x();
	void rotate_global_y();

private:
	glm::mat4 m_model_mat;
};

