#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>
#include <ew/model.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
	ew::Vec3 position = ew::Vec3(0);
	ew::Vec3 color = ew::Vec3(1.0f);
	float radius = 1.0f;
};

#define MAX_LIGHTS 4
Light lights[MAX_LIGHTS];
int numLights = MAX_LIGHTS;
bool orbitLights = true;
float orbitRadius = 2.0;

struct Material {
	float ambientK = 0.1f;
	float diffuseK = 0.4f;
	float specularK = 0.5f;
	float shininess = 64.0f;
}material;

struct PerFrameData {
	ew::Mat4 viewProj = ew::IdentityMatrix();
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	ew::Shader postProcessShader("assets/fsQuad.vert", "assets/postProcess.frag");

	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int concreteTexture = ew::loadTexture("assets/concrete_color.jpg",GL_REPEAT,GL_LINEAR);


	//Load meshes
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 16));
	ew::Model suzanneModel("assets/Suzanne.obj");

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;

	planeTransform.position = ew::Vec3(0, -1.0, 0);
	cubeTransform.position = ew::Vec3(0, 0.5, 0);

	resetCamera(camera,cameraController);

	ew::Vec3 lightColors[MAX_LIGHTS]{
		ew::Vec3(1.0f,0.0f,0.0f),
		ew::Vec3(0.0f,1.0f,0.0f),
		ew::Vec3(0.0f,0.0f,1.0f),
		ew::Vec3(1.0f,1.0f,0.0f),
	};
	float thetaStep = ew::TAU / MAX_LIGHTS;
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		float theta = i * thetaStep;
		lights[i].position = ew::Vec3(cos(theta) * orbitRadius, 2.0, sin(theta) * orbitRadius);
		lights[i].color = lightColors[i];
	}


	//Create framebuffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//Color attachment
	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	//Depth atttachment
	unsigned int depthBuffer;
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);

	GLenum success = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (success != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer incomplete %d", success);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Create uniform buffer
	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);
	GLuint perFrameDataBuffer;
	glCreateBuffers(1, &perFrameDataBuffer);
	glNamedBufferStorage(perFrameDataBuffer, kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer, 0, kUniformBufferSize);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//Update model
		cubeTransform.position.y = 0.5f + sinf(time) * 0.2f;
		//cubeTransform.Rotate(ew::Vec3(0, 1, 0), deltaTime);
		cubeTransform.rotation *= ew::AngleAxis(ew::Vec3(0, 1, 0), deltaTime);
		//Update lights
		if (orbitLights) {
			float thetaStep = ew::TAU / MAX_LIGHTS;
			for (int i = 0; i < MAX_LIGHTS; i++)
			{
				float theta = i * thetaStep + time * 0.5f;
				lights[i].position = ew::Vec3(cos(theta) * orbitRadius, 2.0, sin(theta) * orbitRadius);
			}
		}
		
		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Global uniform buffer
		PerFrameData perFrameData;
		perFrameData.viewProj = camera.ProjectionMatrix() * camera.ViewMatrix();
		glNamedBufferSubData(perFrameDataBuffer, 0, kUniformBufferSize, &perFrameData);

		shader.use();
		shader.setInt("_Texture", 0);
		shader.setVec3("_EyePos", camera.position);
		shader.setFloat("_Material.ambientK", material.ambientK);
		shader.setFloat("_Material.diffuseK", material.diffuseK);
		shader.setFloat("_Material.specularK", material.specularK);
		shader.setFloat("_Material.shininess", material.shininess);

		shader.setInt("_NumLights", numLights);
		for (size_t i = 0; i < numLights; i++)
		{
			shader.setVec3("_Lights["+std::to_string(i)+"].position", lights[i].position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
		}
		
		//Draw ground
		glBindTextureUnit(0, brickTexture);
		shader.setMat4("_Model", planeTransform.ModelMatrix());
		planeMesh.Draw();
		
		//Draw suzanne
		glBindTextureUnit(0, concreteTexture);
		shader.setMat4("_Model", cubeTransform.ModelMatrix());
		suzanneModel.Draw(shader);

		//Render point lights
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		unlitShader.setVec3("_EyePos", camera.position);
		for (size_t i = 0; i < numLights; i++)
		{
			ew::Mat4 model = ew::Translate(lights[i].position) * ew::Scale(0.2f);
			unlitShader.setMat4("_Model", model);
			unlitShader.setVec3("_Color", lights[i].color);
			sphereMesh.Draw();
		}

		//Draw fullscreen quad to screen
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		postProcessShader.use();
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		postProcessShader.setInt("_ColorBuffer", 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);*/

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::SliderInt("Num Lights", &numLights, 0, MAX_LIGHTS);
			ImGui::Checkbox("Orbit Lights", &orbitLights);
			ImGui::DragFloat("Orbit Radius", &orbitRadius, 0.025f);

			for (int i = 0; i < numLights; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Light")) {
					ImGui::DragFloat3("Position", &lights[i].position.x, 0.1f);
					ImGui::ColorEdit3("Color", &lights[i].color.x);
				}
				ImGui::PopID();
			}
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::SliderFloat("AmbientK", &material.ambientK, 0.0f, 1.0f);
				ImGui::SliderFloat("DiffuseK", &material.diffuseK, 0.0f, 1.0f);
				ImGui::SliderFloat("SpecularK", &material.specularK, 0.0f, 1.0f);
				ImGui::SliderFloat("Shininess", &material.shininess, 2.0f, 1024.0f);
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


