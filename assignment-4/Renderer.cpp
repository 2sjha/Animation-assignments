#include "Renderer.h"
#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <nanogui/vector.h>

Camera* Renderer::m_camera = new Camera();

Lighting* Renderer::m_lightings = new Lighting();

nanogui::Screen* Renderer::m_nanogui_screen = nullptr;

Curve* Renderer::m_curve = new Curve();

Aircraft_Animation* Renderer::m_aircraft_animation = new Aircraft_Animation();

bool Renderer::keys[1024];

Renderer::Renderer()
{
}


Renderer::~Renderer()
{	
}

void Renderer::nanogui_init(GLFWwindow* window)
{
	m_nanogui_screen = new nanogui::Screen();
	m_nanogui_screen->initialize(window, true);

	glViewport(0, 0, m_camera->width, m_camera->height);

	//glfwSwapInterval(0);
	//glfwSwapBuffers(window);

	// Create nanogui gui
	nanogui::FormHelper *gui_1 = new nanogui::FormHelper(m_nanogui_screen);
	nanogui::ref<nanogui::Window> nanoguiWindow_1 = gui_1->add_window(nanogui::Array<int, 2>(0, 0), "sxj220028 Assignment-4");

	//screen->setPosition(Eigen::Vector2i(-width/2 + 200, -height/2 + 300));

	gui_1->add_group("Camera Position");
	static auto camera_x_widget = gui_1->add_variable("X", m_camera->position[0]);
	static auto camera_y_widget = gui_1->add_variable("Y", m_camera->position[1]);
	static auto camera_z_widget = gui_1->add_variable("Z", m_camera->position[2]);

	gui_1->add_button("Reset Camera", []() {
		m_camera->reset();
		camera_x_widget->set_value(m_camera->position[0]);
		camera_y_widget->set_value(m_camera->position[1]);
		camera_z_widget->set_value(m_camera->position[2]);
	});

	gui_1->add_group("Curve Simulation");
	gui_1->add_variable("Catmull-Rom curve ON/OFF", m_curve->enabled);
	gui_1->add_button("Change Curve", [this]() {
		this->load_models();
	});

	gui_1->add_group("Aircraft Animation");
	gui_1->add_variable("t1", m_aircraft_animation->t1);
	gui_1->add_variable("t2", m_aircraft_animation->t2);
	gui_1->add_variable("Total moving time", m_aircraft_animation->total_moving_time);
	gui_1->add_button("Move Aircraft", [this]() {
		this->m_aircraft_animation->init(m_curve);
		m_aircraft_animation->start_moving();
	});
	gui_1->add_button("Reset Aircraft Movement", []() {
		m_aircraft_animation->reset();
	});

	m_nanogui_screen->set_visible(true);
	m_nanogui_screen->perform_layout();


	
	glfwSetCursorPosCallback(window,
		[](GLFWwindow *window, double x, double y) {
		m_nanogui_screen->cursor_pos_callback_event(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		m_nanogui_screen->mouse_button_callback_event(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
		//screen->keyCallbackEvent(key, scancode, action, mods);

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				keys[key] = true;
			else if (action == GLFW_RELEASE)
				keys[key] = false;
		}
		camera_x_widget->set_value(m_camera->position[0]);
		camera_y_widget->set_value(m_camera->position[1]);
		camera_z_widget->set_value(m_camera->position[2]);
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		m_nanogui_screen->char_callback_event(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		m_nanogui_screen->drop_callback_event(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		m_nanogui_screen->scroll_callback_event(x, y);
		//m_camera->ProcessMouseScroll(y);
	}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		m_nanogui_screen->resize_callback_event(width, height);
	}
	);

}

void Renderer::init()
{
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#if defined(__APPLE__)
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_camera->init();
	m_lightings->init();
	m_curve->init();
	m_aircraft_animation->init(m_curve);

	// Create a GLFWwindow object that we can use for GLFW's functions
	this->m_window = glfwCreateWindow(m_camera->width, m_camera->height, "Assignment 4", nullptr, nullptr);
	glfwMakeContextCurrent(this->m_window);

	glewExperimental = GL_TRUE;
	glewInit();

	nanogui_init(this->m_window);
}

void Renderer::display(GLFWwindow* window)
{
	Shader m_shader = Shader("./shader/basic.vert", "./shader/basic.frag");

	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();

		if (is_scene_reset) {
			scene_reset();
			is_scene_reset = false;
		}

		camera_move();

		m_shader.use();
			
		setup_uniform_values(m_shader);

		draw_scene(m_shader);

		m_nanogui_screen->draw_widgets();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return;
}

void Renderer::run()
{
	init();
	display(this->m_window);
}

void Renderer::load_models()
{
	obj_list.clear();
	Object cube_object("./objs/cube.obj");
	cube_object.obj_color = glm::vec4(1.0, 1.0, 0.0, 1.0);
	cube_object.obj_name = "cube";

	Object plane_object("./objs/plane.obj");
	plane_object.obj_color = glm::vec4(0.5, 0.5, 0.5, 1.0);
	plane_object.obj_name = "plane";

	Object arrow_object("./objs/arrow.obj");
	arrow_object.obj_name = "axis_arrow";

	Object aircraft_object("./objs/aircraft.obj");
	aircraft_object.obj_color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	aircraft_object.obj_name = "aircraft";

	m_curve->calculate_curve();

	Object curve_object(m_curve->curve_points_pos);
	curve_object.m_render_type = RENDER_LINES;
	curve_object.obj_color = glm::vec4(1.0, 0.0, 0.0,1.0);
	curve_object.obj_name = "curve";


	bind_vaovbo(cube_object);
	bind_vaovbo(plane_object);
	bind_vaovbo(arrow_object);
	bind_vaovbo(aircraft_object);
	bind_vaovbo(curve_object);
	
	// Here we only load one model
	obj_list.push_back(cube_object);
	obj_list.push_back(plane_object);
	obj_list.push_back(arrow_object);
	obj_list.push_back(aircraft_object);
	obj_list.push_back(curve_object);
}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glFrontFace(GL_CW);

	for (size_t i = 0; i < obj_list.size(); i++)
	{
		if (obj_list[i].obj_name == "cube")
		{
			// Draw objects
			for (unsigned int j = 0; j < m_curve->control_points_pos.size(); j++) {
				glm::mat4 cur_obj_model_mat = glm::mat4(1.0f);

				cur_obj_model_mat = glm::translate(cur_obj_model_mat, m_curve->control_points_pos[j]);

				cur_obj_model_mat = glm::scale(cur_obj_model_mat, glm::vec3(0.4, 0.4, 0.4));

				// Rotate Cube according to their respective quat
				glm::quat curr_quaternion = m_curve->control_points_quaternion[j];
				glm::mat4 rotation_mat = glm::mat4_cast(curr_quaternion);
				cur_obj_model_mat = cur_obj_model_mat * rotation_mat;

				glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(cur_obj_model_mat));
				draw_object(shader, obj_list[i]);

				// Draw axis for each cube
				draw_axis(shader,cur_obj_model_mat);
			}

		}
		if (obj_list[i].obj_name == "aircraft")
		{
			glm::mat4 cur_aircraft_model_mat = glm::mat4(1.0f);
			m_aircraft_animation->update(delta_time);
			cur_aircraft_model_mat = m_aircraft_animation->get_model_mat();

			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(cur_aircraft_model_mat));

			draw_object(shader, obj_list[i]);

			// Draw axis for aircraft
			draw_axis(shader,cur_aircraft_model_mat);
		}

		if (obj_list[i].obj_name == "plane")
		{
			// Draw plane
			glm::mat4 plane_model_mat =  glm::mat4(1.0f);
			plane_model_mat = glm::scale(plane_model_mat, glm::vec3(10, 10, 10));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(plane_model_mat));
			draw_object(shader, obj_list[i]);
		}

		if (obj_list[i].obj_name == "axis_arrow")
		{
			glm::mat4 world_identity_obj_mat = glm::mat4(1.0f);
			draw_axis(shader, world_identity_obj_mat);
		}

		if (obj_list[i].obj_name == "curve") 
		{
			// Draw curve
			glm::mat4 curve_model_mat =  glm::mat4(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(curve_model_mat));
			draw_object(shader, obj_list[i]);
		}
	}
}


void Renderer::camera_move()
{
	GLfloat current_frame = glfwGetTime();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	// Camera controls
	if (keys[GLFW_KEY_W])
		m_camera->process_keyboard(FORWARD, delta_time);
	if (keys[GLFW_KEY_S])
		m_camera->process_keyboard(BACKWARD, delta_time);
	if (keys[GLFW_KEY_A])
		m_camera->process_keyboard(LEFT, delta_time);
	if (keys[GLFW_KEY_D])
		m_camera->process_keyboard(RIGHT, delta_time);
	if (keys[GLFW_KEY_Q])
		m_camera->process_keyboard(UP, delta_time);
	if (keys[GLFW_KEY_E])
		m_camera->process_keyboard(DOWN, delta_time);
	if (keys[GLFW_KEY_I])
		m_camera->process_keyboard(ROTATE_X_UP, delta_time);
	if (keys[GLFW_KEY_K])
		m_camera->process_keyboard(ROTATE_X_DOWN, delta_time);
	if (keys[GLFW_KEY_J])
		m_camera->process_keyboard(ROTATE_Y_UP, delta_time);
	if (keys[GLFW_KEY_L])
		m_camera->process_keyboard(ROTATE_Y_DOWN, delta_time);
	if (keys[GLFW_KEY_U])
		m_camera->process_keyboard(ROTATE_Z_UP, delta_time);
	if (keys[GLFW_KEY_O])
		m_camera->process_keyboard(ROTATE_Z_DOWN, delta_time);

}

void Renderer::draw_object(Shader& shader, Object& object)
{
	glBindVertexArray(object.vao);

	glUniform3f(glGetUniformLocation(shader.program, "m_object.object_color"), object.obj_color[0], object.obj_color[1], object.obj_color[2]);
	glUniform1f(glGetUniformLocation(shader.program, "m_object.shininess"), object.shininess);

	if (object.m_render_type == RENDER_TRIANGLES)
	{
		if (object.m_obj_type == OBJ_POINTS)
		{
			std::cout << "Error: Cannot render triangles if input obj type is point\n";
			return;
		}
		if (object.m_obj_type == OBJ_TRIANGLES)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, object.vao_vertices.size());
		}
	}

	if (object.m_render_type == RENDER_LINES)
	{
		glLineWidth(10.0);
		if (object.m_obj_type == OBJ_POINTS)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_LINE_LOOP, 0, object.vao_vertices.size());
		}
		if (object.m_obj_type == OBJ_TRIANGLES)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, object.vao_vertices.size());
		}
	}

	if (object.m_obj_type == OBJ_POINTS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
		glDrawArrays(GL_POINTS, 0, object.vao_vertices.size());
	}
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::draw_axis(Shader& shader, const glm::mat4 axis_obj_mat)
{
	// You can always see the arrow
	glDepthFunc(GL_ALWAYS);
	// Get arrow obj
	Object *cur_obj = nullptr;
	for (unsigned int i = 0; i < obj_list.size(); i++)
	{
		if (obj_list[i].obj_name == "axis_arrow") {
			cur_obj = &obj_list[i];
		}
	}

	if (cur_obj == nullptr)
		return;

	// Draw main axis
	glm::mat4 model_mat_x = axis_obj_mat;
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model_mat_x));
	cur_obj->obj_color = glm::vec4(1, 0, 0, 1);
	draw_object(shader, *cur_obj);

	glm::mat4 model_mat_y = axis_obj_mat;
	model_mat_y = glm::rotate(model_mat_y, glm::radians(90.0f), glm::vec3(0, 0, 1));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model_mat_y));
	cur_obj->obj_color = glm::vec4(0, 1, 0, 1);
	draw_object(shader, *cur_obj);

	glm::mat4 model_mat_z = axis_obj_mat;
	model_mat_z = glm::rotate(model_mat_z, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(model_mat_z));
	cur_obj->obj_color = glm::vec4(0, 0, 1, 1);
	draw_object(shader, *cur_obj);
	glDepthFunc(GL_LESS);
}

void Renderer::bind_vaovbo(Object &cur_obj)
{
	glGenVertexArrays(1, &cur_obj.vao);
	glGenBuffers(1, &cur_obj.vbo);

	glBindVertexArray(cur_obj.vao);

	glBindBuffer(GL_ARRAY_BUFFER, cur_obj.vbo);
	glBufferData(GL_ARRAY_BUFFER, cur_obj.vao_vertices.size() * sizeof(Object::Vertex), &cur_obj.vao_vertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, TexCoords));

	glBindVertexArray(0);
}

void Renderer::setup_uniform_values(Shader& shader)
{
	// Camera uniform values
	glUniform3f(glGetUniformLocation(shader.program, "camera_pos"), m_camera->position.x, m_camera->position.y, m_camera->position.z);

	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(m_camera->get_projection_mat()));
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(m_camera->get_view_mat()));

	// Light uniform values
	glUniform1i(glGetUniformLocation(shader.program, "dir_light.status"), m_lightings->direction_light.status);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.direction"), m_lightings->direction_light.direction[0], m_lightings->direction_light.direction[1], m_lightings->direction_light.direction[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.ambient"), m_lightings->direction_light.ambient[0], m_lightings->direction_light.ambient[1], m_lightings->direction_light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.diffuse"), m_lightings->direction_light.diffuse[0], m_lightings->direction_light.diffuse[1], m_lightings->direction_light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.specular"), m_lightings->direction_light.specular[0], m_lightings->direction_light.specular[1], m_lightings->direction_light.specular[2]);

	// Set current point light as camera's position
	m_lightings->point_light.position = m_camera->position;
	glUniform1i(glGetUniformLocation(shader.program, "point_light.status"), m_lightings->point_light.status);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.position"), m_lightings->point_light.position[0], m_lightings->point_light.position[1], m_lightings->point_light.position[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.ambient"), m_lightings->point_light.ambient[0], m_lightings->point_light.ambient[1], m_lightings->point_light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.diffuse"), m_lightings->point_light.diffuse[0], m_lightings->point_light.diffuse[1], m_lightings->point_light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.specular"), m_lightings->point_light.specular[0], m_lightings->point_light.specular[1], m_lightings->point_light.specular[2]);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.constant"), m_lightings->point_light.constant);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.linear"), m_lightings->point_light.linear);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.quadratic"), m_lightings->point_light.quadratic);
}

void Renderer::scene_reset()
{
	load_models();
	m_camera->reset();
}