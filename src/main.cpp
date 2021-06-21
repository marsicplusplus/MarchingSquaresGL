#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <iostream>
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
int g_res = 100;

std::vector<Point3> g_gridPos;
std::vector<Point3> g_isoLines;
std::vector<float> g_isoColors;
std::vector<float> g_gridVal;

GLuint g_valueVBO, g_posVBO, g_gridVAO, g_isolineVBO, g_isoColorVBO, g_isolineVAO;

GLFWwindow* initGL();
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint setupGrid();
void setGridValues();
GLuint calcIsolines();

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
	
	GLuint g_gridVAO = setupGrid();
	GLuint g_isolineVAO = calcIsolines();

	Shader shader;
	shader.loadShader("shaders/vert.glsl", GL_VERTEX_SHADER);
	shader.loadShader("shaders/frag.glsl", GL_FRAGMENT_SHADER);
	shader.compileShaders();

	glPointSize(2.0f);
	glLineWidth(3.0f);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	const double fpsLimit = 1.0/12.0;
	double lastUpdateTime = 0;
	double lastFrameTime = 0;
	while(!glfwWindowShouldClose(window)){
		double now = glfwGetTime();

		glfwPollEvents();

		if((now - lastFrameTime) >= fpsLimit) {
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, g_winWidth, g_winHeight);
			shader.use();

			glBindVertexArray(g_gridVAO);
			glDrawArrays(GL_POINTS, 0, g_gridPos.size());
			glBindVertexArray(0);

			glBindVertexArray(g_isolineVAO);
			glDrawArrays(GL_LINES, 0, g_isoLines.size()/2);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			//setGridValues();
			lastFrameTime = now;
		}
		lastUpdateTime = now;
	}
	shader.deleteProgram();
	glfwTerminate();
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

	GLFWwindow* window = glfwCreateWindow(g_winWidth, g_winHeight, "MarchingSquaresGL", NULL, NULL);
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

GLuint setupGrid() {
	int rows = g_winHeight / g_res;
	float quadHeight = 2.0f/static_cast<float>(rows);
	int cols = g_winWidth / g_res;
	float quadWidth = 2.0f/static_cast<float>(cols);
	for(int i = 0; i < rows+ 1; i++){
		float y = -1.0f + i * quadWidth;
		for(int j = 0; j < cols + 1; j++){
			float x = -1.0f + j * quadHeight;
			printf("x: %f - y: %f\n", x, y);
			g_gridPos.push_back({x, y, 0.0f});
			int val = std::rand() % 2;
			g_gridVal.push_back(val);
			printf("val: %d\n", val);
		}
	}
	glGenBuffers(1, &g_posVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_posVBO);
	glBufferData(GL_ARRAY_BUFFER, g_gridPos.size() * sizeof(Point3), &g_gridPos[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &g_gridVAO);
	glBindVertexArray(g_gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_posVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &g_valueVBO);
	setGridValues();

	glGenBuffers(1, &g_isolineVBO);
	glGenBuffers(1, &g_isoColorVBO);
	glGenVertexArrays(1, &g_isolineVAO);
	return g_gridVAO;
}

void setGridValues(){
	int rows = g_winHeight / g_res;
	int cols = g_winWidth / g_res;
	//g_gridVal.clear();
	//for(int i = 0; i < rows + 1; i++){
		//for(int j = 0; j < cols + 1; j++){
			//int val = std::rand() % 2;
			//g_gridVal.push_back(val);
			//printf("val: %d\n", val);
		//}
	//}

	glBindBuffer(GL_ARRAY_BUFFER, g_valueVBO);
	glBufferData(GL_ARRAY_BUFFER, g_gridVal.size() * sizeof(float), &g_gridVal[0], GL_STATIC_DRAW);

	glBindVertexArray(g_gridVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_valueVBO);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
}

int getState(int a, int b, int c, int d) {
	return d + c * 2 + b * 4 + a * 8;
}

GLuint calcIsolines(){
	int rows = g_winHeight / g_res;
	int cols = g_winWidth / g_res;
	float quadHeight = 2.0f/static_cast<float>(rows);
	float quadWidth = 2.0f/static_cast<float>(cols);
	// TODO: FIGURE OUT THOSE LOOPS!
	for(int i = 0; i < rows; i++){
		std::cout << "---" << std::endl;
		for(int j = 0; j < cols; j++){
			float a = g_gridVal[(rows + 1) * i + j];
			float b = g_gridVal[(rows + 1) * (i+1) + j];
			float c = g_gridVal[(rows + 1) * (i+1) + (j+1)];
			float d = g_gridVal[(rows + 1) * (i) + (j+1)];
			int state = getState(a, b, c, d);
			std::cout << state << std::endl;
			//g_isoLines.push_back(std::rand() % 2);
			g_isoColors.push_back(1.0f);
			switch(state) {
				case 0:
					break;
				case 1:
					Point3 pt = g_gridPos[(rows+1) * i + j];
					std::cout << "Draw Line from ("<< pt.x << ", " << pt.y - quadHeight / 2 << ") to (" << pt.x + quadWidth / 2 << ", " << pt.y - quadHeight  << ");" << std::endl;
					g_isoLines.push_back({pt.x, pt.y - quadHeight / 2});
					g_isoLines.push_back({pt.x + quadWidth / 2, pt.y - quadHeight});
					std::cout << "DRAW LINE!" << std::endl;
					break;
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, g_isolineVBO);
	glBufferData(GL_ARRAY_BUFFER, g_isoLines.size() * sizeof(Point3), &g_isoLines[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_isoColorVBO);
	glBufferData(GL_ARRAY_BUFFER, g_isoColors.size() * sizeof(float), &g_isoColors[0], GL_STATIC_DRAW);

	glBindVertexArray(g_isolineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_isolineVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, g_isoColorVBO);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
}
