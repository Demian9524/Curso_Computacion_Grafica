//práctica #3
//Bello Zaragoza Demian
//Fecha de entrega 5 septiembre 2026
//Número de cuenta 320200928

#include<iostream>

//#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

const GLint WIDTH = 800, HEIGHT = 600;

int main() {
	glfwInit();

	//Verificación de compatibilidad
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);*/

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(
		WIDTH,
		HEIGHT,
		"Práctica 3 Bello Zaragoza Demian",
		nullptr,
		nullptr
	);

	int screenWidth, screenHeight;

	glfwGetFramebufferSize(
		window,
		&screenWidth,
		&screenHeight
	);

	//Verificación de errores de creación de ventana
	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;

	//Verificación de errores de inicialización de GLEW
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	//Define las dimensiones del viewport
	glViewport(0, 0, screenWidth, screenHeight);

	//Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	//Enable alpha support
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Build and compile our shader program
	Shader ourShader(
		"Shader/core.vs",
		"Shader/core.frag"
	);

	//Use with Perspective Projection
	float vertices[] = {

		//Front
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,

		//Back
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

		//Right
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,

		 //Left
		 -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,

		 //Bottom
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,

		 //Top
		 -0.5f, 0.5f, -0.5f, 1.0f, 0.2f, 0.5f,
		  0.5f, 0.5f, -0.5f, 1.0f, 0.2f, 0.5f,
		  0.5f, 0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
		  0.5f, 0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
		 -0.5f, 0.5f,  0.5f, 1.0f, 0.2f, 0.5f,
		 -0.5f, 0.5f, -0.5f, 1.0f, 0.2f, 0.5f
	};

	GLuint VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	//Enlazar Vertex Array Object
	glBindVertexArray(VAO);

	//Copiamos el arreglo de vértices en un buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);

	//Posición
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(GLfloat),
		(GLvoid*)0
	);

	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(GLfloat),
		(GLvoid*)(3 * sizeof(GLfloat))
	);

	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 projection = glm::mat4(1);

	projection = glm::perspective(
		45.0f,
		(GLfloat)screenWidth / (GLfloat)screenHeight,
		0.1f,
		100.0f
	);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		//Fondo
		glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.Use();

		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = glm::mat4(1);

		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -12.0f));

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");

		glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glBindVertexArray(VAO);
		glDisableVertexAttribArray(1);


		//Caja 1 roja
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(-0.85f, -2.70f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(8.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(28.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-6.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(1.80f, 0.90f, 1.20f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 1.0f, 0.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Caja 2 azul
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(0.05f, -1.55f, 0.10f)
		);

		model = glm::rotate(
			model,
			glm::radians(-12.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-18.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(9.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.80f, 1.25f, 0.85f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 0.0f, 0.30f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Caja 3 verde
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(0.45f, -0.52f, -0.15f)
		);

		model = glm::rotate(
			model,
			glm::radians(6.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(35.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-10.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(1.55f, 0.95f, 1.10f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 0.0f, 1.0f, 0.10f);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Caja 4 amarilla
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(-0.05f, 0.72f, 0.05f)
		);

		model = glm::rotate(
			model,
			glm::radians(-16.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(12.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(0.75f, 1.25f, 0.75f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 1.0f, 1.0f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Caja 5 rosa
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(-0.70f, 1.70f, -0.10f)
		);

		model = glm::rotate(
			model,
			glm::radians(10.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(24.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-7.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(2.00f, 0.65f, 1.00f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Caja 6 naranja
		model = glm::mat4(1);

		model = glm::translate(
			model,
			glm::vec3(0.35f, 2.65f, 0.12f)
		);

		model = glm::rotate(
			model,
			glm::radians(-14.0f),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(-25.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		model = glm::rotate(
			model,
			glm::radians(28.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		model = glm::scale(
			model,
			glm::vec3(1.25f, 1.05f, 0.90f)
		);

		glUniformMatrix4fv(
			modelLoc,
			1,
			GL_FALSE,
			glm::value_ptr(model)
		);

		glVertexAttrib3f(1, 1.0f, 0.35f, 0.0f);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return EXIT_SUCCESS;
}