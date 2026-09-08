//previo práctica #4
//Bello Zaragoza Demian
//Fecha de entrega 07 septiembre 2026
//Número de cuenta 320200928

#include <iostream>
#include <cstdlib>

//#define GLEW_STATIC

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

// Tiempo entre fotogramas.
float deltaTime = 0.0f;
double tiempoAnterior = 0.0;

int main()
{
    if (!glfwInit())
    {
        std::cout << "Error al inicializar GLFW." << std::endl;
        return EXIT_FAILURE;
    }

    // Opciones de compatibilidad del codigo original.
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    */

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WIDTH, HEIGHT, "Modelado geometrico", nullptr, nullptr
    );

    // Primero comprobar que la ventana se creo correctamente.
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    // Cubo base: posicion (X, Y, Z) y color (R, G, B).
    float vertices[] = {
        // Frente: rojo.
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        // Atras: verde.
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

        // Derecha: azul.
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         // Izquierda: amarillo.
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

         // Abajo: cian.
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

         // Arriba: rosa.
         -0.5f,  0.5f, -0.5f,  1.0f, 0.2f, 0.5f,
          0.5f,  0.5f, -0.5f,  1.0f, 0.2f, 0.5f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.2f, 0.5f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.2f, 0.5f,
         -0.5f,  0.5f,  0.5f,  1.0f, 0.2f, 0.5f,
         -0.5f,  0.5f, -0.5f,  1.0f, 0.2f, 0.5f
    };

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // Posicion.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (GLvoid*)0
    );
    glEnableVertexAttribArray(0);

    // Color.
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);

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
    GLint projecLoc = glGetUniformLocation(
        ourShader.Program, "projection"
    );

    tiempoAnterior = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // Calcular el tiempo transcurrido entre fotogramas.
        double tiempoActual = glfwGetTime();
        deltaTime = static_cast<float>(tiempoActual - tiempoAnterior);
        tiempoAnterior = tiempoActual;

        // Evitar saltos grandes si el programa se pausa.
        if (deltaTime > 0.05f)
            deltaTime = 0.05f;

        glfwPollEvents();
        Inputs(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        view = glm::translate(
            view, glm::vec3(movX, movY, movZ)
        );

        view = glm::rotate(
            view,
            glm::radians(rot),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glUniformMatrix4fv(
            projecLoc, 1, GL_FALSE, glm::value_ptr(projection)
        );

        glUniformMatrix4fv(
            viewLoc, 1, GL_FALSE, glm::value_ptr(view)
        );

        glBindVertexArray(VAO);

        // TABLERO
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(3.0f, 0.3f, 2.0f));
        model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // PATA 1: derecha, adelante
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        model = glm::translate(model, glm::vec3(4.5f, -0.45f, 2.833333f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // PATA 2: izquierda, adelante
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        model = glm::translate(model, glm::vec3(-4.5f, -0.45f, 2.833333f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // PATA 3: izquierda, atras
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        model = glm::translate(model, glm::vec3(-4.5f, -0.45f, -2.833333f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // PATA 4: derecha, atras
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.6f, 0.3f));
        model = glm::translate(model, glm::vec3(4.5f, -0.45f, -2.833333f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    // Cambia estos valores para ajustar la velocidad.
    const float velocidadMovimiento = 0.6f;  // Unidades por segundo.
    const float velocidadRotacion = 20.0f;   // Grados por segundo.

    float paso = velocidadMovimiento * deltaTime;
    float giro = velocidadRotacion * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Movimiento horizontal.
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movX += paso;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movX -= paso;

    // Subir: Page Up o flecha arriba.
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        movY += paso;
    }

    // Bajar: Page Down o flecha abajo.
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        movY -= paso;
    }

    // Alejar.
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movZ -= paso;

    // Acercar.
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movZ += paso;

    // Rotar.
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot += giro;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot -= giro;
}