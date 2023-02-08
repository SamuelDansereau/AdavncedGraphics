#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/ShapeGen.h"

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;

glm::vec3 bgColor = glm::vec3(0);




struct Camera {
	glm::vec3 pos = glm::vec3(0, 0, 10);
	glm::vec3 targ = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	float orbitRadius = 10.0f;
	float orbitSpeed = 1.0f;
	float fov = 50.0f;
	float orthoHeight = 10.0f;
	bool orthoTogg = true;
	
	glm::mat4 getViewMatrix()
	{
		glm::vec3 forward = glm::normalize(targ - pos);
		glm::vec3 right = glm::normalize(cross(forward, up));
		glm::vec3 upwards = glm::normalize(cross(right, forward));
		forward = -forward;
		glm::mat4 rotView = glm::mat4(
			right.x, upwards.x, forward.x, 0,
			right.y, upwards.y, forward.y, 0,
			right.z, upwards.z, forward.z, 0,
			0,0,0,1
		);

		glm::mat4 transView = glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-pos.x, - pos.y, -pos.z, 1
		);

		return rotView * transView;
	}
	glm::mat4 getProjectonMatrix()
	{
		if (orthoTogg)
		{
			return ortho(orthoHeight, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, .01f, 100.0f);
		}
		else
			return persp(fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, .01f, 100.0f);
	}

	glm::mat4 ortho(float h, float aspectRatio, float nearPlane, float farPlane)
	{
		float r = h*aspectRatio/2;
		float l = -r;
		float t = h / 2;
		float b = -t;

		return glm::mat4
		(
			2 / (r - l), 0, 0, 0,
			0, 2 / (t - b), 0, 0,
			0, 0, -2 / (farPlane - nearPlane), 0,
			-(r + l) / (r - l), -(t + b) / (t - b), -(farPlane + nearPlane) / (farPlane - nearPlane), 1
		);
	}

	glm::mat4 persp(float fovy, float aspectRatio, float nearPlane, float farPlane)
	{
		float c = tan(glm::radians(fovy) / 2);
		return glm::mat4
		(
			1 / (aspectRatio * c), 0, 0, 0,
			0, 1 / c, 0, 0,
			0, 0, -((farPlane + nearPlane)/(farPlane-nearPlane)), -1,
			0, 0, -((2*farPlane*nearPlane)/(farPlane-nearPlane)), 1
		);
	}
};

struct Transform {

	glm::vec3 pos, rot, scl;

	glm::mat4 getModelMatrix() {
		return position(pos) * rotate(rot) * scale(scl);
	}
	glm::mat4 scale(glm::vec3 s)
	{

		return glm::mat4
		(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	}
	glm::mat4 rotate(glm::vec3 r)
	{
		glm::mat4 pitch = glm::mat4
		(
			1, 0, 0, 0,
			0, cos(r.x), sin(r.x), 0,
			0, -sin(r.x), cos(r.x), 0,
			0, 0, 0, 1
		);
		glm::mat4 yaw = glm::mat4
		(
			cos(r.y), 0, -sin(r.y), 0,
			0, 1, 0, 0,
			sin(r.y), 0, cos(r.y), 0,
			0, 0, 0, 1
		);
		glm::mat4 roll = glm::mat4
		(
			cos(r.z), sin(r.z), 0, 0,
			-sin(r.z), cos(r.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
		return pitch * yaw * roll;
	}
	glm::mat4 position(glm::vec3 t)
	{
		return glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			t.x, t.y, t.z, 1
		);
	}

};



int main() {
	Transform transforms[5];
	Camera cam;
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	MeshData cubeMeshData;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	Mesh cubeMesh(&cubeMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	for (int i = 0; i < 5; i++)
	{
		int randScale = (rand() % 3) + 1;
		transforms[i].pos = glm::vec3((rand() % (i+5)), (rand() % (i+5)), (rand() % (i+5)));
		transforms[i].rot = glm::vec3((rand() % (181+1)), (rand() % (181+i)), (rand() % (181+i)));
		transforms[i].scl = glm::vec3(randScale, randScale, randScale);
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//Draw
		shader.use();
		/*for (size_t i = 0; i < NUM_CUBES; i++) {
			shader.setMat4("_Model", transforms[i].getModelMatrix());
			cubeMesh.draw();
		}*/

		for (int i = 0; i < 5; i++)
		{
			shader.setMat4("_Model", transforms[i].getModelMatrix());
			cubeMesh.draw();
		}
		shader.setMat4("_Projection", cam.getProjectonMatrix());
		shader.setMat4("_View", cam.getViewMatrix());
		
		cam.pos.x = cos(time * cam.orbitSpeed) * cam.orbitRadius;
		cam.pos.z = sin(time * cam.orbitSpeed) * cam.orbitRadius;

		//Draw UI
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Orbit Radius", &cam.orbitRadius, 10.0f, 25.0f);
		ImGui::SliderFloat("Orbit Speed", &cam.orbitSpeed, 0.0f, 10.0f);
		ImGui::SliderFloat("FOV", &cam.fov, 0.0f, 150.0f);
		ImGui::SliderFloat("Orthographic Height", &cam.orthoHeight, 10.0f, 25.0f);
		ImGui::Checkbox("Orthographic Toggle", &cam.orthoTogg);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}