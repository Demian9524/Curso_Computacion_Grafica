// previo práctica #4
// Bello Zaragoza Demian
// Fecha de entrega 07 septiembre 2026
// Número de cuenta 320200928

#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

void Inputs(GLFWwindow* window);

const GLint WIDTH = 800, HEIGHT = 600;

float movX = 0.0f;
float movY = 0.0f;
float movZ = -5.0f;
float rot = 0.0f;

float deltaTime = 0.0f;
double tiempoAnterior = 0.0;

int main()
{
    if (!glfwInit())
    {
        std::cout << "Error al inicializar GLFW." << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WIDTH, HEIGHT, "Modelado geometrico", nullptr, nullptr
    );

    if (window == nullptr)
    {
        std::cout << "Error al crear la ventana." << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error al inicializar GLEW." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    // Cubo base: solamente posiciones X, Y, Z.
    float vertices[] = {
        // Frente
        -0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,  -0.5f, -0.5f,  0.5f,

        // Atrás
        -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,

        // Derecha
         0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,   0.5f, -0.5f,  0.5f,

         // Izquierda
         -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f,
         -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,
         -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,

         // Abajo
         -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,  -0.5f, -0.5f, -0.5f,

         // Arriba
         -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,   0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f
    };

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(GLfloat), (GLvoid*)0
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        (GLfloat)screenWidth / (GLfloat)screenHeight,
        0.1f,
        100.0f
    );

    GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
    GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
    GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");
    GLint colorLoc = glGetUniformLocation(ourShader.Program, "objectColor");

    tiempoAnterior = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double tiempoActual = glfwGetTime();
        deltaTime = static_cast<float>(tiempoActual - tiempoAnterior);
        tiempoAnterior = tiempoActual;

        if (deltaTime > 0.05f)
            deltaTime = 0.05f;

        glfwPollEvents();
        Inputs(window);

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(view, glm::vec3(movX, movY, movZ));

        view = glm::rotate(
            view, glm::radians(rot),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glUniformMatrix4fv(
            projecLoc, 1, GL_FALSE, glm::value_ptr(projection)
        );

        glUniformMatrix4fv(
            viewLoc, 1, GL_FALSE, glm::value_ptr(view)
        );

        glBindVertexArray(VAO);

        // Función reutilizable: dibuja un cubo con el modelo y color indicados.
        auto dibujarCubo = [&](glm::mat4 modelo, glm::vec3 color)
            {
                glUniformMatrix4fv(
                    modelLoc, 1, GL_FALSE, glm::value_ptr(modelo)
                );

                glUniform3fv(colorLoc, 1, glm::value_ptr(color));

                glDrawArrays(GL_TRIANGLES, 0, 36);
            };

        // Cabeza: blanca.
        glm::mat4 model = glm::mat4(1.0f);//una declaracion de model
        model = glm::scale(model, glm::vec3(0.8f, 1.45f, 0.77f));
        model = glm::translate(model, glm::vec3(0.0f, 0.28f, 0.0f));
        dibujarCubo(model, glm::vec3(0.8706f, 0.8706f, 0.8706f));

        // Ojo derecho
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.225f, 0.25f, 0.1f));
        model = glm::translate(model, glm::vec3(1.2f, 3.0f, 4.0f));
        dibujarCubo(model, glm::vec3(0.0f, 0.0f, 0.0f));

        // Ojo izquierdo
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.225f, 0.25f, 0.1f));
        model = glm::translate(model, glm::vec3(-1.2f, 3.0f, 4.0f));
        dibujarCubo(model, glm::vec3(0.0f, 0.0f, 0.0f));

        // Pico arriba.
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.8f, 0.25f, 0.6f));
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.0f));
        dibujarCubo(model, glm::vec3(0.749f, 0.573f, 0.259f));

        // Pico abajo.
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.8f, 0.25f, 0.6f));
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 1.0f));
        dibujarCubo(model, glm::vec3(0.573f, 0.447f, 0.192f));

        // Papada.
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.4f, 0.65f, 0.3f));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f));
        dibujarCubo(model, glm::vec3(1.0f, 0.0f, 0.0f));

        // Cuerpo
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 1.45f, 1.77f));
        model = glm::translate(model, glm::vec3(1.0f, 1.28f, 1.0f));
        dibujarCubo(model, glm::vec3(0.8706f, 0.8706f, 0.8706f));

        //// PATA 2: naranja.
        //model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        //model = glm::translate(model, glm::vec3(-4.5f, -0.45f, 2.833333f));
        //dibujarCubo(model, glm::vec3(1.0f, 0.5f, 0.0f));

        //// PATA 3: naranja.
        //model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        //model = glm::translate(model, glm::vec3(-4.5f, -0.45f, -2.833333f));
        //dibujarCubo(model, glm::vec3(1.0f, 0.5f, 0.0f));

        //// PATA 4: naranja.
        //model = glm::mat4(1.0f);
        //model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        //model = glm::translate(model, glm::vec3(4.5f, -0.45f, -2.833333f));
        //dibujarCubo(model, glm::vec3(1.0f, 0.5f, 0.0f));

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void Inputs(GLFWwindow* window)
{
    const float velocidadMovimiento = 0.6f;
    const float velocidadRotacion = 20.0f;

    float paso = velocidadMovimiento * deltaTime;
    float giro = velocidadRotacion * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movX += paso;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movX -= paso;

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        movY += paso;

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        movY -= paso;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movZ -= paso;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movZ += paso;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot += giro;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot -= giro;
}//pequena caratula, con numero de equipo NO NOMBRES COMPLETOS, presentacion de equipo: nombre integrantes, nombre equipo, numero de cuenta y correo de los integrantes,PALETA DE COLORES, TIPOGRAFIA