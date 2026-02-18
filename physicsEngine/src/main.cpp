#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "../buffer.h"
#include "../shader.h"

// Vertex Shader
const char* vertexShaderCode = "#version 330 core\n"
"layout(location = 0) in vec3 inPosition;\n"
"layout(location = 1) in vec3 inColor;\n"
"out vec4 outColor;\n"
"uniform mat4 model;\n" // <--- THE NEW UNIFORM
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"	gl_Position = projection * view * model * vec4(inPosition, 1.0f);\n" // <--- MULTIPLY HERE
"	outColor = vec4(inColor, 1.0f);\n"
"};\n";

// Fragment Shader
const char* fragmentShaderCode = "#version 330 core\n"
"in vec4 outColor;\n"
"out vec4 glFragColor;\n"
"void main() {\n"
"	glFragColor = outColor;\n"
"};\n";

// Camera State
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Physics Variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float playerVelocityY = 0.0f;
bool isGrounded = true;

// Mouse Movement
float yaw = -90.0f; // Start facing toward -Z
float pitch = 0.0f;
float lastX = 400, lastY = 350; // Middle of your 800x700 window
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Constrain the pitch so you don't flip your head over
	if (pitch > 89.0f)  pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	// Convert angles to a Direction Vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

int main(void)
{
	// GLFW & GLAD Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 700, "WINDOW", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Compiling Shaders -> Shader Program
	Shader myShader(vertexShaderCode, fragmentShaderCode);

	// Vertex Data
	float cubeVertices[] = {
		// Positions          // Colors
		// Bottom Face (Red)
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

		// Front Face (Red)
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		// Back Face (Red)
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

		// Left Face (Red)
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		// Right Face (Red)
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		 // Top Face (Ground - Green)
		 -0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.0f,
		  0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.0f,
		  0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,
		  0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,
		 -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,
		 -0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.0f
	};

	// Vertex Buffer & Vertex Array Initialization
	Buffer myBuffer(cubeVertices, sizeof(cubeVertices));

	// Back Buffer Clear Color
	glClearColor(0.5f, 0.8f, 0.9f, 1.0f);

	// Mouse Logic
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// PHYISCS
	float ballY = 5.0f;
	float velocityY = 0.0f;

	// Matrix Locations
	unsigned int modelLoc = glGetUniformLocation(myShader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");
	unsigned int projectionLoc = glGetUniformLocation(myShader.ID, "projection");

	// Game Loop while window is open
	while (!glfwWindowShouldClose(window))
	{
		// TIMING
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// I/O
		glm::vec3 walkFront = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
		float cameraSpeed = 2.5f * deltaTime;
		playerVelocityY -= 9.8f * deltaTime; // Player Gravity
		cameraPos.y += playerVelocityY * deltaTime; // Player Gravity

		if (cameraPos.y <= 1.0f) { // Floor level for the camera
			cameraPos.y = 1.0f;
			playerVelocityY = 0.0f;
			isGrounded = true;
		}

		// Jump!
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded) {
			playerVelocityY = 5.0f;
			isGrounded = false;
		}

		// KEYS
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			cameraPos += cameraSpeed * walkFront;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			cameraPos -= cameraSpeed * walkFront;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		glfwPollEvents();

		// PHYSICS LOGIC
		velocityY -= 9.8f * deltaTime; // Gravity
		ballY += velocityY * deltaTime;
		if (ballY < 0.5f) { // Collision with ground (assuming ball/cube size)
			ballY = 0.5f;
			velocityY *= -0.8f; // Bounce!
		}

		// 4. RENDERING 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		myShader.use();


		// SET VIEW/PROJECTION
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / 700, 0.1f, 1000.f);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		myBuffer.bind();

		// DRAW GROUND
		glm::mat4 model = glm::mat4(1.0f);
		// Move it slightly down so y=0 is the top surface
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // Trnaslate before scaling.. not sure why
		// Scale it wide (100) and flat (0.01)
		model = glm::scale(model, glm::vec3(100.0f, 0.01f, 100.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// DRAW RED BOX
		glm::mat4 ballModel = glm::mat4(1.0f);
		ballModel = glm::translate(ballModel, glm::vec3(0.0f, ballY, -5.0f)); // Move it forward so we can see it
		// No scaling because cube is already a 1x1
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ballModel));
		glDrawArrays(GL_TRIANGLES, 0, 36); // Draw the same geometry, but moved!

		// Swap Back -> Front Buffer
		glfwSwapBuffers(window);

	}

	// Delete allocated GPU VRAM
	glDeleteVertexArrays(1, &myBuffer.VAO);
	glDeleteBuffers(1, &myBuffer.VBO);
	glDeleteProgram(myShader.ID);
	glfwTerminate();

	return 0;
}