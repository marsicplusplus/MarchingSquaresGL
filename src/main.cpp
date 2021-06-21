#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <vector>

#include "shader.hpp"

typedef struct{
	float x;
	float y;
	float z;
} Point3;

int g_winWidth = 640.0f;
int g_winHeight = 640.0f;

int g_res = 40;

GLFWwindow* initGL();
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
	std::srand(std::time(nullptr));
	GLFWwindow *window = initGL();
	if(!window){
		fprintf(stderr, "ERROR: something bad happened during GLFW initialization, exiting...\n");
		return -1;
	}
	glfwSetKeyCallback(window, keyboardCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		glfwTerminate();
		return -1;
	}
	
	std::vector<Point3> gridPos;
	std::vector<float> gridVal;
	int rows = 1 + g_winHeight / g_res;
	float quadHeight = 2.0f/static_cast<float>(rows);
	int cols = 1 + g_winWidth / g_res;
	float quadWidth = 2.0f/static_cast<float>(cols);
	for(int i = 0; i < rows; i++){
		float x = -1.0f + i * quadHeight;
		for(int j = 0; j < cols; j++){
			float y = -1.0f + j * quadWidth;
			gridPos.push_back({x, y, 0.0f});
			gridVal.push_back(std::rand() % 2);
		}
	}
	
	GLuint posVBO, valueVBO, gridVAO;
	glGenBuffers(1, &posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glBufferData(GL_ARRAY_BUFFER, gridPos.size() * sizeof(Point3), &gridPos[0], GL_STATIC_DRAW);

	glGenBuffers(1, &valueVBO);
	glBindBuffer(GL_ARRAY_BUFFER, valueVBO);
	glBufferData(GL_ARRAY_BUFFER, gridVal.size() * sizeof(float), &gridVal[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &gridVAO);
	glBindVertexArray(gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, valueVBO);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	Shader shader;
	shader.loadShader("shaders/vert.glsl", GL_VERTEX_SHADER);
	shader.loadShader("shaders/frag.glsl", GL_FRAGMENT_SHADER);
	shader.compileShaders();

	glPointSize(5.0f);
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, g_winWidth, g_winHeight);
		shader.use();
	
		glBindVertexArray(gridVAO);
		glDrawArrays(GL_POINTS, 0, gridPos.size());
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
}

GLFWwindow* initGL(){
	if(!glfwInit()){
		fprintf(stderr, "ERROR: cannot start GLFW3\n");
		return nullptr;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
	if(!window) {
		fprintf(stderr, "ERROR: could not create GLFW3 window\n");
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	return window;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, 1);
}
