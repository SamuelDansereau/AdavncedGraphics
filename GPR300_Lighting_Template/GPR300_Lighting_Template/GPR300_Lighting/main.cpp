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
#include "EW/EwMath.h"
#include "EW/Camera.h"
#include "EW/Mesh.h"
#include "EW/Transform.h"
#include "EW/ShapeGen.h"

void processInput(GLFWwindow* window);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

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
const float CAMERA_MOVE_SPEED = 5.0f;
const float CAMERA_ZOOM_SPEED = 3.0f;

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

glm::vec3 bgColor = glm::vec3(0);
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);

bool wireFrame = false;

struct Material
{
	glm::vec3 Color = glm::vec3(250,250,250);
	float ambientK = 0.4, diffuseK = 0.16, specularK = 0.9;
	float shininess = 1;
};


struct Light {
	glm::vec3 pos;
	glm::vec3 color;
	float intensity;
};

struct DirLight {
	glm::vec3 dir = glm::vec3(0,-1,0);
	glm::vec3 color = glm::vec3(250, 250, 250);
	float intensity = 1.0;
};

struct PointLight {
	glm::vec3 pos = glm::vec3(1);
	glm::vec3 color;
	float intensity;
	float radius;
};

struct SpotLight {
	glm::vec3 pos = glm::vec3(1);
	glm::vec3 dir = glm::vec3(0, -1, 0);
	glm::vec3 color = glm::vec3(250, 250, 250);;
	float intensity = 0;
	float linearAttenuation;
	float minAnlge, maxAnlge;

};

#define MAX_LIGHTS 2
DirLight dlight;
PointLight plights[MAX_LIGHTS];
SpotLight slight;

Material mat;

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lighting", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	//Used to draw shapes. This is the shader you will be completing.
	Shader litShader("shaders/defaultLit.vert", "shaders/defaultLit.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	ew::MeshData cubeMeshData;
	ew::createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	ew::MeshData sphereMeshData;
	ew::createSphere(0.5f, 64, sphereMeshData);
	ew::MeshData cylinderMeshData;
	ew::createCylinder(1.0f, 0.5f, 64, cylinderMeshData);
	ew::MeshData planeMeshData;
	ew::createPlane(1.0f, 1.0f, planeMeshData);

	ew::Mesh cubeMesh(&cubeMeshData);
	ew::Mesh sphereMesh(&sphereMeshData);
	ew::Mesh planeMesh(&planeMeshData);
	ew::Mesh cylinderMesh(&cylinderMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Initialize shape transforms
	ew::Transform cubeTransform;
	ew::Transform sphereTransform;
	ew::Transform planeTransform;
	ew::Transform cylinderTransform;
	ew::Transform lightTransform;

	cubeTransform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
	sphereTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);

	planeTransform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	planeTransform.scale = glm::vec3(10.0f);

	cylinderTransform.position = glm::vec3(2.0f, 0.0f, 0.0f);

	lightTransform.scale = glm::vec3(0.5f);
	lightTransform.position = glm::vec3(0.0f, 5.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//Draw
		litShader.use();
		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setVec3("_LightPos", lightTransform.position);

		litShader.setVec3("camPos", camera.getPosition());

		litShader.setVec3("_Material.Color", mat.Color);
		litShader.setFloat("_Material.ambinetK", mat.ambientK);
		litShader.setFloat("_Material.diffuseK", mat.diffuseK);
		litShader.setFloat("_Material.specularK", mat.specularK);
		litShader.setFloat("_Material.shininess", mat.shininess);

		litShader.setVec3("_dLight.dir", dlight.dir);
		litShader.setVec3("_dLight.color", dlight.color);
		litShader.setFloat("_dLight.intensity", dlight.intensity);

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			litShader.setVec3("_pLight[i].pos", plights[i].pos);
			litShader.setVec3("_pLight[i].color", plights[i].color);
			litShader.setFloat("_pLight[i].intensity", plights[i].intensity);
			litShader.setFloat("_pLight[i].radius", plights[i].radius);
		}

		litShader.setVec3("_sLight.pos", slight.pos);
		litShader.setVec3("_sLight.dir", slight.dir);
		litShader.setVec3("_sLight.color", slight.color);
		litShader.setFloat("_sLight.intensity", slight.intensity);
		litShader.setFloat("_sLight.linearAttenuation", slight.linearAttenuation);
		litShader.setFloat("_sLight.minAnlge", slight.minAnlge);
		litShader.setFloat("_sLight.maxAnlge", slight.maxAnlge);

		//Draw cube
		litShader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		//Draw sphere
		litShader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		//Draw cylinder
		litShader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Draw plane
		litShader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		//Draw light as a small sphere using unlit shader, ironically.
		unlitShader.use();
		unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
		unlitShader.setMat4("_View", camera.getViewMatrix());
		unlitShader.setMat4("_Model", lightTransform.getModelMatrix());
		unlitShader.setVec3("_Color", lightColor);
		sphereMesh.draw();

		//Draw UI
		ImGui::Begin("Material");

		ImGui::ColorEdit3("Material Color", &mat.Color.r);
		ImGui::SliderFloat("Ambient",&mat.ambientK, 0, 1);
		ImGui::SliderFloat("Diffuse", &mat.diffuseK, 0, 1);
		ImGui::SliderFloat("Specular", &mat.specularK, 0, 1);
		ImGui::SliderFloat("Shininess", &mat.shininess, 1, 512);

		ImGui::End();

		ImGui::Begin("Directional Light");
		
		ImGui::ColorEdit3("Light Color", &dlight.color.r);
		ImGui::DragFloat3("Light Direction", &dlight.dir.x);
		ImGui::SliderFloat("Light Intensity", &dlight.intensity, 0, 1);
		
		ImGui::End();

		ImGui::Begin("Point Light 1");

		ImGui::DragFloat3("Light Position", &plights[0].pos.x);
		ImGui::ColorEdit3("Light Color", &plights[0].color.r);
		ImGui::SliderFloat("Light Intensity", &plights[0].intensity, 0, 1);
		ImGui::SliderFloat("radius", &plights[0].radius, 0, 52);

		ImGui::End();

		ImGui::Begin("Point Light 2");

		ImGui::DragFloat3("Light Position", &plights[1].pos.x);
		ImGui::ColorEdit3("Light Color", &plights[1].color.r);
		ImGui::SliderFloat("Light Intensity", &plights[1].intensity, 0, 1);
		ImGui::SliderFloat("radius", &plights[1].radius, 0, 52);

		ImGui::End();

		ImGui::Begin("Spot Light");

		ImGui::DragFloat3("Light Position", &slight.pos.x);
		ImGui::DragFloat3("Light Direction", &slight.dir.x);
		ImGui::ColorEdit3("Light Color", &slight.color.r);
		ImGui::SliderFloat("Light Intensity", &slight.intensity, 0, 1);
		ImGui::SliderFloat("Linear Attenuation", &slight.linearAttenuation, 0, 52);
		ImGui::SliderFloat("Max Angle", &slight.maxAnlge, 0, 90);
		ImGui::SliderFloat("Min Angle", &slight.minAnlge, 0, 90);


		ImGui::End();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
//Author: Eric Winebrenner
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);
	glViewport(0, 0, width, height);
}
//Author: Eric Winebrenner
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Reset camera
	if (keycode == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.setPosition(glm::vec3(0, 0, 5));
		camera.setYaw(-90.0f);
		camera.setPitch(0.0f);
		firstMouseInput = false;
	}
	if (keycode == GLFW_KEY_1 && action == GLFW_PRESS) {
		wireFrame = !wireFrame;
		glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
	}
}
//Author: Eric Winebrenner
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (abs(yoffset) > 0) {
		float fov = camera.getFov() - (float)yoffset * CAMERA_ZOOM_SPEED;
		camera.setFov(fov);
	}
}
//Author: Eric Winebrenner
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}
	if (!firstMouseInput) {
		prevMouseX = xpos;
		prevMouseY = ypos;
		firstMouseInput = true;
	}
	float yaw = camera.getYaw() + (float)(xpos - prevMouseX) * MOUSE_SENSITIVITY;
	camera.setYaw(yaw);
	float pitch = camera.getPitch() - (float)(ypos - prevMouseY) * MOUSE_SENSITIVITY;
	pitch = glm::clamp(pitch, -89.9f, 89.9f);
	camera.setPitch(pitch);
	prevMouseX = xpos;
	prevMouseY = ypos;
}
//Author: Eric Winebrenner
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//Toggle cursor lock
	if (button == MOUSE_TOGGLE_BUTTON && action == GLFW_PRESS) {
		int inputMode = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, inputMode);
		glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
	}
}

//Author: Eric Winebrenner
//Returns -1, 0, or 1 depending on keys held
float getAxis(GLFWwindow* window, int positiveKey, int negativeKey) {
	float axis = 0.0f;
	if (glfwGetKey(window, positiveKey)) {
		axis++;
	}
	if (glfwGetKey(window, negativeKey)) {
		axis--;
	}
	return axis;
}

//Author: Eric Winebrenner
//Get input every frame
void processInput(GLFWwindow* window) {

	float moveAmnt = CAMERA_MOVE_SPEED * deltaTime;

	//Get camera vectors
	glm::vec3 forward = camera.getForward();
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	glm::vec3 position = camera.getPosition();
	position += forward * getAxis(window, GLFW_KEY_W, GLFW_KEY_S) * moveAmnt;
	position += right * getAxis(window, GLFW_KEY_D, GLFW_KEY_A) * moveAmnt;
	position += up * getAxis(window, GLFW_KEY_Q, GLFW_KEY_E) * moveAmnt;
	camera.setPosition(position);
}
