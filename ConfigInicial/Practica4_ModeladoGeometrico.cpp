// práctica #4
// Bello Zaragoza Demian
// Fecha de entrega 12 septiembre 2026
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
        WIDTH, HEIGHT, "Practica #4 Bello Zaragoza Demian", nullptr, nullptr
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
    GLint scaleLoc = glGetUniformLocation(ourShader.Program, "objectScale");

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

        glClearColor(0.62f, 0.80f, 0.98f, 1.0f);
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

        // Función reutilizable: ahora también recibe la escala.
        auto dibujarCubo = [&](glm::mat4 modelo, glm::vec3 color, glm::vec3 escala)
            {
                glUniformMatrix4fv(
                    modelLoc, 1, GL_FALSE, glm::value_ptr(modelo)
                );

                glUniform3fv(
                    colorLoc, 1, glm::value_ptr(color)
                );

                glUniform3fv(
                    scaleLoc, 1, glm::value_ptr(escala)
                );

                glDrawArrays(GL_TRIANGLES, 0, 36);
            };

        // Cabeza: blanca.
        glm::vec3 escalaCabeza(0.90f, 1.375f, 0.77f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, escalaCabeza);
        model = glm::translate(model, glm::vec3(0.0f, 0.3f, 0.0f));

        dibujarCubo(
            model,
            glm::vec3(0.8706f, 0.8706f, 0.8706f),
            escalaCabeza
        );

        // Ojo derecho
        glm::vec3 escalaOjo(0.225f, 0.25f, 0.1f);

        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaOjo);
        model = glm::translate(model, glm::vec3(1.5f, 3.0f, 4.0f));

        dibujarCubo(
            model,
            glm::vec3(0.0f, 0.0f, 0.0f),
            escalaOjo
        );

        // Ojo izquierdo
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaOjo);
        model = glm::translate(model, glm::vec3(-1.5f, 3.0f, 4.0f));

        dibujarCubo(
            model,
            glm::vec3(0.0f, 0.0f, 0.0f),
            escalaOjo
        );

        // Pico arriba.
        glm::vec3 escalaPico(0.9f, 0.20f, 0.55f);

        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPico);
        model = glm::translate(model, glm::vec3(0.0f, 2.6f, 1.1f));

        dibujarCubo(
            model,
            glm::vec3(0.749f, 0.573f, 0.259f),
            escalaPico
        );

        // Pico abajo.
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPico);
        model = glm::translate(model, glm::vec3(0.0f, 1.6f, 1.1f));

        dibujarCubo(
            model,
            glm::vec3(0.573f, 0.447f, 0.192f),
            escalaPico
        );

        // Papada.
        glm::vec3 escalaPapada(0.4f, 0.5f, 0.35f);

        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPapada);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.3f));

        dibujarCubo(
            model,
            glm::vec3(1.0f, 0.0f, 0.0f),
            escalaPapada
        );

        // Cuerpo
        glm::vec3 escalaCuerpo(1.375f, 1.375f, 1.83f);

        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaCuerpo);
        model = glm::translate(model, glm::vec3(0.0f, -0.325f, -0.55f));

        dibujarCubo(
            model,
            glm::vec3(0.8706f, 0.8706f, 0.8706f),
            escalaCuerpo
        );

        // Ala Derecha
        glm::vec3 escalaAla(0.275f, 0.916f, 1.375f);

        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaAla);
        model = glm::translate(model, glm::vec3(-3.0f, -0.29f, -0.75f));

        dibujarCubo(
            model,
            glm::vec3(0.8706f, 0.8706f, 0.8706f),
            escalaAla
        );

        // Ala Izquerda
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaAla);
        model = glm::translate(model, glm::vec3(3.0f, -0.29f, -0.75f));

        dibujarCubo(
            model,
            glm::vec3(0.8706f, 0.8706f, 0.8706f),
            escalaAla
        );

        // Patita Derecha
        glm::vec3 escalaPatita(0.275f, 1.2f, 0.1f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPatita);
        model = glm::translate(model, glm::vec3(1.25f, -1.425f, -9.50f));

        dibujarCubo(
            model,
            glm::vec3(0.9529f, 0.8627f, 0.4196f),
            escalaPatita
        );

        // Patita Izquierda
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPatita);
        model = glm::translate(model, glm::vec3(-1.25f, -1.425f, -9.50f));

        dibujarCubo(
            model,
            glm::vec3(0.9529f, 0.8627f, 0.4196f),
            escalaPatita
        );


        // Pies
        glm::vec3 escalaPies(1.2f, 0.1f, 0.5f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaPies);
        model = glm::translate(model, glm::vec3(0.0f, -23.0f, -1.35f));

        dibujarCubo(
            model,
            glm::vec3(0.9529f, 0.8627f, 0.4196f),
            escalaPies
        );

        // Dedo del pie derecho
        glm::vec3 escalaDedoPie(0.25f, 0.1f, 0.25f);
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaDedoPie);
        model = glm::translate(model, glm::vec3(1.25f, -23.0f, -1.25f));

        dibujarCubo(
            model,
            glm::vec3(0.9529f, 0.8627f, 0.4196f),
            escalaDedoPie
        );

        // Dedo del pie izquierdo
        model = glm::mat4(1.0f);
        model = glm::scale(model, escalaDedoPie);
        model = glm::translate(model, glm::vec3(-1.25f, -23.0f, -1.25f));

        dibujarCubo(
            model,
            glm::vec3(0.9529f, 0.8627f, 0.4196f),
            escalaDedoPie
        );

        // Capa de tierra
        glm::vec3 escalaTierra(30.0f, 1.0f, 30.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.92f, 0.0f));
        model = glm::scale(model, escalaTierra);

        dibujarCubo(
            model,
            glm::vec3(0.45f, 0.30f, 0.14f),   // color base tierra
            escalaTierra
        );


        // Capa superior de pasto
        glm::vec3 escalaPasto(30.0f, 1.0f, 30.0f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.9f, 0.0f));
        model = glm::scale(model, escalaPasto);

        dibujarCubo(
            model,
            glm::vec3(0.35f, 0.65f, 0.18f),   // color base pasto
            escalaPasto
        );

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
    const float velocidadMovimiento = 1.0f;
    const float velocidadRotacion = 50.0f;

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
}

// pequena caratula, con numero de equipo NO NOMBRES COMPLETOS,
// presentacion de equipo: nombre integrantes, nombre equipo,
// numero de cuenta y correo de los integrantes,
// PALETA DE COLORES, TIPOGRAFIA
