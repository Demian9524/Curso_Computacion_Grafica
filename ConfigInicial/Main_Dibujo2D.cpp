//práctica #2
//Bello Zaragoza Demian
//Fecha de entrega 29 agosto 2026
//Número de cuenta 320200928

#include <iostream>
#include <vector>

//#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Shaders
#include "Shader.h"

struct Color {
    float r, g, b;
};

void agregarRect(std::vector<float>& vertices,
    std::vector<unsigned int>& indices,
    int col, int fila,
    int ancho, int alto,
    int totalCols, int totalFilas,
    Color c)
{
    float x1 = -1.0f + 2.0f * col / totalCols;
    float x2 = -1.0f + 2.0f * (col + ancho) / totalCols;

    float y1 = 1.0f - 2.0f * fila / totalFilas;
    float y2 = 1.0f - 2.0f * (fila + alto) / totalFilas;

    unsigned int base = static_cast<unsigned int>(vertices.size() / 6);

    float rect[] = {
        x1, y1, 0.0f, c.r, c.g, c.b,   // A
        x2, y1, 0.0f, c.r, c.g, c.b,   // B
        x2, y2, 0.0f, c.r, c.g, c.b,   // C
        x1, y2, 0.0f, c.r, c.g, c.b    // D
    };

    vertices.insert(vertices.end(), rect, rect + 24);

    unsigned int inds[] = {
        base, base + 1, base + 2,
        base, base + 2, base + 3
    };

    indices.insert(indices.end(), inds, inds + 6);
}

void resize(GLFWwindow* window, int width, int height);

const GLint WIDTH = 800, HEIGHT = 600;

int main()
{
    glfwInit();

    // Verificacion de compatibilidad
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    */

    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Práctica #2 Bello Zaragoza Demian",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resize);

    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialise GLEW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Informacion de OpenGL del sistema
    std::cout << "> Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "> Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "> Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "> SL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Aseguramos el viewport inicial
    glViewport(0, 0, WIDTH, HEIGHT);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const int COLS = 29;
    const int FILAS = 29;

    Color negro = { 0.0f, 0.0f, 0.0f };
    Color verdeOscuro = { 0.16f, 0.37f, 0.00f };
    Color verdeMedio = { 0.43f, 0.71f, 0.08f };
    Color verdeClaro = { 0.70f, 0.86f, 0.20f };\
    Color verdeBrillante = { 0.26f, 0.86f, 0.05f };
    Color crema = { 0.90f, 0.90f, 0.70f };

    //CABEZA

    // fila 1
    agregarRect(vertices, indices, 9, 0, 6, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 20, 0, 4, 1, COLS, FILAS, negro);

    // fila 2
    agregarRect(vertices, indices, 7, 1, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 9, 1, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 11, 1, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 13, 1, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 1, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 19, 1, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 20, 1, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 21, 1, 2, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 23, 1, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 1, 1, 1, COLS, FILAS, negro);

    // fila 3
    agregarRect(vertices, indices, 6, 2, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 7, 2, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 8, 2, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 9, 2, 6, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 15, 2, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 16, 2, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 17, 2, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 19, 2, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 20, 2, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 21, 2, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 22, 2, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 24, 2, 1, 1, COLS, FILAS, negro);

    // fila 4
    agregarRect(vertices, indices, 5, 3, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 3, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 7, 3, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 8, 3, 1, 1, COLS, FILAS, crema);
    agregarRect(vertices, indices, 9, 3, 7, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 16, 3, 1, 1, COLS, FILAS, crema);
    agregarRect(vertices, indices, 17, 3, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 18, 3, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 19, 3, 2, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 21, 3, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 3, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 23, 3, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 24, 3, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 25, 3, 1, 1, COLS, FILAS, negro);

    // fila 5
    agregarRect(vertices, indices, 2, 4, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 5, 4, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 6, 4, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 7, 4, 2, 1, COLS, FILAS, crema);
    agregarRect(vertices, indices, 9, 4, 4, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 13, 4, 1, 1, COLS, FILAS, crema);
    agregarRect(vertices, indices, 14, 4, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 4, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 16, 4, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 17, 4, 4, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 21, 4, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 4, 3, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 25, 4, 1, 1, COLS, FILAS, negro);

    // fila 6
    agregarRect(vertices, indices, 1, 5, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 2, 5, 3, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 5, 5, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 6, 5, 2, 1, COLS, FILAS, crema);
    agregarRect(vertices, indices, 8, 5, 5, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 13, 5, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 5, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 16, 5, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 17, 5, 4, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 21, 5, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 5, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 5, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 25, 5, 1, 1, COLS, FILAS, negro);

    // fila 7
    agregarRect(vertices, indices, 0, 6, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 1, 6, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 2, 6, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 3, 6, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 4, 6, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 5, 6, 8, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 13, 6, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 6, 6, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 21, 6, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 6, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 6, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 25, 6, 1, 1, COLS, FILAS, negro);

    // fila 8
    agregarRect(vertices, indices, 0, 7, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 1, 7, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 2, 7, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 3, 7, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 4, 7, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 5, 7, 16, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 21, 7, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 7, 3, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 25, 7, 1, 1, COLS, FILAS, negro);

    // fila 9
    agregarRect(vertices, indices, 0, 8, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 1, 8, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 2, 8, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 5, 8, 12, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 17, 8, 3, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 20, 8, 2, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 22, 8, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 23, 8, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 24, 8, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 25, 8, 1, 1, COLS, FILAS, negro);

    // fila 10
    agregarRect(vertices, indices, 1, 9, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 4, 9, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 5, 9, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 6, 9, 10, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 16, 9, 5, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 21, 9, 1, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 22, 9, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 24, 9, 1, 1, COLS, FILAS, negro);

    // fila 11
    agregarRect(vertices, indices, 4, 10, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 5, 10, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 6, 10, 8, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 14, 10, 4, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 18, 10, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 19, 10, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 20, 10, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 21, 10, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 23, 10, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 10, 1, 1, COLS, FILAS, negro);

    // fila 12
    agregarRect(vertices, indices, 5, 11, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 11, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 7, 11, 3, 1, COLS, FILAS, verdeClaro);
    agregarRect(vertices, indices, 10, 11, 7, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 17, 11, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 18, 11, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 20, 11, 4, 1, COLS, FILAS, negro);

    // fila 13
    agregarRect(vertices, indices, 6, 12, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 7, 12, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 8, 12, 7, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 15, 12, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 17, 12, 1, 1, COLS, FILAS, negro);

    // fila 14
    agregarRect(vertices, indices, 7, 13, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 9, 13, 6, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 13, 2, 1, COLS, FILAS, negro);

    //TALLO
    // fila 15
    agregarRect(vertices, indices, 9, 14, 6, 1, COLS, FILAS, negro);

    // fila 16
    agregarRect(vertices, indices, 11, 15, 3, 1, COLS, FILAS, negro);

    // fila 17
    agregarRect(vertices, indices, 10, 16, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 11, 16, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 12, 16, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 13, 16, 1, 1, COLS, FILAS, negro);

    // fila 18
    agregarRect(vertices, indices, 8, 17, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 11, 17, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 12, 17, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 13, 17, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 15, 17, 7, 1, COLS, FILAS, negro);

    // fila 19
    agregarRect(vertices, indices, 7, 18, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 9, 18, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 10, 18, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 11, 18, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 12, 18, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 13, 18, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 14, 18, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 15, 18, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 16, 18, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 17, 18, 5, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 22, 18, 1, 1, COLS, FILAS, negro);

    // fila 20
    agregarRect(vertices, indices, 7, 19, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 10, 19, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 11, 19, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 12, 19, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 13, 19, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 14, 19, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 15, 19, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 17, 19, 5, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 22, 19, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 23, 19, 1, 1, COLS, FILAS, negro);

    // fila 21
    agregarRect(vertices, indices, 6, 20, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 7, 20, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 9, 20, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 11, 20, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 12, 20, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 13, 20, 3, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 16, 20, 7, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 23, 20, 2, 1, COLS, FILAS, negro);

    // fila 22
    agregarRect(vertices, indices, 5, 21, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 21, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 7, 21, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 8, 21, 3, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 11, 21, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 13, 21, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 21, 6, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 21, 21, 1, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 22, 21, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 23, 21, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 21, 1, 1, COLS, FILAS, negro);

    // fila 23
    agregarRect(vertices, indices, 5, 22, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 22, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 7, 22, 6, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 13, 22, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 15, 22, 1, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 16, 22, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 18, 22, 3, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 21, 22, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 22, 22, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 23, 22, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 22, 1, 1, COLS, FILAS, negro);

    // fila 24
    agregarRect(vertices, indices, 5, 23, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 23, 8, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 14, 23, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 16, 23, 3, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 19, 23, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 20, 23, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 23, 23, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 24, 23, 1, 1, COLS, FILAS, negro);

    // fila 25
    agregarRect(vertices, indices, 5, 24, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 24, 8, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 14, 24, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 16, 24, 2, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 18, 24, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 19, 24, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 22, 24, 3, 1, COLS, FILAS, negro);

    // fila 26
    agregarRect(vertices, indices, 5, 25, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 25, 1, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 7, 25, 3, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 10, 25, 4, 1, COLS, FILAS, verdeBrillante);
    agregarRect(vertices, indices, 14, 25, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 15, 25, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 16, 25, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 17, 25, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 18, 25, 2, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 23, 25, 1, 1, COLS, FILAS, negro);

    // fila 27
    agregarRect(vertices, indices, 5, 26, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 6, 26, 1, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 7, 26, 3, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 10, 26, 1, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 11, 26, 2, 1, COLS, FILAS, verdeMedio);
    agregarRect(vertices, indices, 13, 26, 2, 1, COLS, FILAS, verdeOscuro);
    agregarRect(vertices, indices, 15, 26, 4, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 22, 26, 1, 1, COLS, FILAS, negro);

    // fila 28
    agregarRect(vertices, indices, 6, 27, 1, 1, COLS, FILAS, negro);
    agregarRect(vertices, indices, 10, 27, 5, 1, COLS, FILAS, negro);

    // fila 29
    agregarRect(vertices, indices, 7, 28, 1, 1, COLS, FILAS, negro);

    std::cout << "Cantidad de floats en vertices: " << vertices.size() << std::endl;
    std::cout << "Cantidad de indices: " << indices.size() << std::endl;

    GLuint VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VBO: copiamos los vertices a la GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    // EBO: copiamos los indices a la GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );

    // Atributo 0: posicion (x, y, z)
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(GLfloat),
        (GLvoid*)0
    );
    glEnableVertexAttribArray(0);

    // Atributo 1: color (r, g, b)
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

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Fondo blanco
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.Use();
        glBindVertexArray(VAO);

        // IMPORTANTE:
        // Dibujamos TODOS los indices, no solo los primeros 6.
        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(indices.size()),
            GL_UNSIGNED_INT,
            0
        );

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // Limpieza
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return EXIT_SUCCESS;
}

void resize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Subiendo ahora la version de dibujo 2D, hubo un pequeno error, debido a que estuve trabajando en el 
//laboratorio sobre el mismo archivo de configuracion inicial, el configuracion inicial original quedo modificado, 
// por lo que ya no puedo acceder a el y subirlo a github, por lo que mejor subo el de dibujo 2D dos veces, el configuracion inicial original quedo inaccesible.