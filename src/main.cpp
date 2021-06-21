#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <vector>
#include "shader.hpp"

struct vec3f{
	float x;
	float y;
	float z;
};
struct vec2f{
	float x;
	float y;
};
struct sphere_t {
	vec2f pos;
	vec2f vel;
	float rad;
	float dist(float x, float y){
		return ((rad*rad)/((x - pos.x)*(x - pos.x) + (y - pos.y)*(y - pos.y)));
	};
};

int g_winWidth = 800.0f;
int g_winHeight = 800.0f;
int g_res = 100;

std::vector<vec3f> g_isolines;
GLuint g_isolineVBO, g_isolineVAO;

std::vector<sphere_t> spheres;

GLFWwindow* initGL();
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void setupGrid();

int main() {
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

	spheres.push_back({{0.0f, 0.0f}, {0.0f, 0.0f}, 0.25f});

	glGenVertexArrays(1, &g_isolineVAO);
	glGenBuffers(1, &g_isolineVBO);

	// TODO: remove this dependency
	Shader shader;
	shader.loadShader("shaders/vert.glsl", GL_VERTEX_SHADER);
	shader.loadShader("shaders/frag.glsl", GL_FRAGMENT_SHADER);
	shader.compileShaders();
	shader.use();

	setupGrid();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	const double fpsLimit = 1.0/12.0;
	double lastUpdateTime = 0;
	double lastFrameTime = 0;
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();

		double now = glfwGetTime();
		double dt = now - lastUpdateTime;

		if((now - lastFrameTime) >= fpsLimit) {
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, g_winWidth, g_winHeight);

			glBindVertexArray(g_isolineVAO);
			glDrawArrays(GL_LINES, 0, g_isolines.size());
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			lastFrameTime = now;
		}
		lastUpdateTime = now;
	}

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

	GLFWwindow* window = glfwCreateWindow(g_winWidth, g_winHeight, "MarchingSquares", NULL, NULL);
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

int getState(int a, int b, int c, int d) {
	return d + c * 2 + b * 4 + a * 8;
}

void setupGrid() {
	float wQuads = g_winWidth / g_res + 1;
	float hQuads = g_winHeight/ g_res + 1;
	float quadHeight = 2.0f/static_cast<float>(hQuads);
	float quadWidth = 2.0f/static_cast<float>(wQuads);
	std::vector<int> val(wQuads * hQuads);
	for(int i = 0; i < hQuads; i++) {
		float y = 2.0f * static_cast<float>(i) / hQuads - 1.0f;
		for(int j = 0; j < hQuads; j++) {
			float x = 2.0f * static_cast<float>(j) / wQuads - 1.0f;
			float res = 0.0f; 
			for(auto s : spheres)
				res += s.dist(x, y);
			val[i * hQuads + j] = res < 1 ? 0 : 1;
			//printf("(%f, %f): %f\n", x, y, val[i * hQuads + j]);
		}
	}
	for(int i = 0; i < hQuads - 1; i++) {
		float y = 2.0f * static_cast<float>(i) / hQuads - 1.0f;
		for(int j = 0; j < hQuads - 1; j++) {
			float x = 2.0f * static_cast<float>(j) / wQuads - 1.0f;
			int a = val[i * hQuads + j];
			int b = val[i * hQuads + j+1];
			int c = val[(i+1) * hQuads + (j+1)];
			int d = val[(i+1) * hQuads + j];
			int state = getState(a, b, c, d);

			switch(state){
				case 0:
				case 15:
					break;
				case 1:
				case 14:
						g_isolines.push_back({x + quadWidth / 2.0f, y, 0.0f});
						g_isolines.push_back({x + quadWidth, y + quadHeight / 2.0f, 0.0f});
					break;
				case 2:
				case 13:
						g_isolines.push_back({x + quadWidth / 2.0f, y + quadHeight, 0.0f});
						g_isolines.push_back({x + quadWidth, y + quadHeight / 2.0f, 0.0f});
					break;
				case 3:
				case 12:
						g_isolines.push_back({x + quadWidth / 2.0f, y + quadHeight, 0.0f});
						g_isolines.push_back({x + quadWidth, y, 0.0f});
					break;
				case 4:
				case 11:
					g_isolines.push_back({x, y + quadHeight / 2.0f, 0.0f});
					g_isolines.push_back({x + quadWidth / 2.0f, y + quadHeight, 0.0f});
					break;
				case 5:
					g_isolines.push_back({x, y + quadHeight / 2.0f, 0.0f});
					g_isolines.push_back({x + quadWidth / 2.0f, y, 0.0f});
					g_isolines.push_back({x+ quadWidth / 2.0f, y + quadHeight, 0.0f});
					g_isolines.push_back({x + quadWidth, y + quadHeight / 2.0f, 0.0f});
					break;
				case 6:
				case 9:
					g_isolines.push_back({x, y + quadHeight / 2.0f, 0.0f});
					g_isolines.push_back({x + quadWidth, y + quadHeight / 2.0f, 0.0f});
					break;
				case 7:
				case 8:
					g_isolines.push_back({x, y + quadHeight / 2.0f, 0.0f});
					g_isolines.push_back({x + quadWidth / 2.0f, y, 0.0f});
					break;
				case 10:
					g_isolines.push_back({x, y + quadWidth / 2.0f, 0.0f});
					g_isolines.push_back({x + quadWidth / 2.0f, y + quadHeight, 0.0f});
					g_isolines.push_back({x+ quadWidth / 2.0f, y, 0.0f});
					g_isolines.push_back({x + quadWidth, y + quadHeight / 2.0f, 0.0f});
					break;
			}
		}
	}
	glBindVertexArray(g_isolineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_isolineVBO);
	glBufferData(GL_ARRAY_BUFFER, g_isolines.size() * sizeof(vec3f), &g_isolines[0], GL_DYNAMIC_DRAW);

	// Position;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);
}
