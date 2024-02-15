#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "shader.h"
#include "nanogui/nanogui.h"
#include <eigen3/Eigen/Dense>

using namespace nanogui;

enum test_enum {
	Item1 = 0,
	Item2,
	Item3
};

// Window dimensions
const GLuint width = 800, height = 600;

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

Screen *screen = nullptr;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "Assignment 0", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
}
	glfwMakeContextCurrent(window);

#if defined(NANOGUI_GLAD)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Could not initialize GLAD!");
	glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Create a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	// Create nanogui gui
	bool enabled = true;
	FormHelper *gui = new FormHelper(screen);
	ref<Window> nanoguiWindow = gui->add_window(Array<int, 2>(10, 10), "Form helper example");
	gui->add_group("Basic types");
	gui->add_variable("bool", bvar)->set_tooltip("Test tooltip.");
	gui->add_variable("string", strval);

	gui->add_group("Validating fields");
	gui->add_variable("int", ivar)->set_spinnable(true);
	gui->add_variable("float", fvar)->set_tooltip("Test.");
	gui->add_variable("double", dvar)->set_spinnable(true);

	gui->add_group("Complex types");
	gui->add_variable("Enumeration", enumval, enabled)->set_items({ "Item 1", "Item 2", "Item 3" });
	gui->add_variable("Color", colval);

	gui->add_group("Other widgets");
	gui->add_button("A button", []() { std::cout << "Button pressed." << std::endl; })->set_tooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

	screen->set_visible(true);
	screen->perform_layout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *, double x, double y) {
		screen->cursor_pos_callback_event(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		screen->mouse_button_callback_event(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *, int key, int scancode, int action, int mods) {
		screen->key_callback_event(key, scancode, action, mods);
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		screen->char_callback_event(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		screen->drop_callback_event(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		screen->scroll_callback_event(x, y);
	}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		screen->resize_callback_event(width, height);
	}
	);


	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();


	// Build and compile our shader program
	OurShader ourShader("shader/basic.vs", "shader/basic.frag");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
		0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top 
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the triangle
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		screen->draw_widgets();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
