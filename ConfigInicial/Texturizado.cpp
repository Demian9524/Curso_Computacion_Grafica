// Práctica #7
// Bello Zaragoza Demian
// Fecha de entrega: 3 octubre 2026
// Número de cuenta: 320200928

#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

// Funciones
void KeyCallback(
    GLFWwindow* window, int key, int scancode, int action, int mode
);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Ventana
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;
int SCREEN_WIDTH = 0;
int SCREEN_HEIGHT = 0;

// Cámara
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool keys[1024] = { false };
bool firstMouse = true;

// Tiempo
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Parámetros del cordoncito.
// Puedes modificar estos valores.
const float ALTURA_ARCO = 1.25f;
const float GROSOR_CORDON = 0.025f;
const int SEGMENTOS_CORDON = 80;

int main()
{
    if (!glfwInit())
    {
        std::cerr << "No se pudo iniciar GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "Practica #7 Bello Zaragoza Demian",
        nullptr,
        nullptr
    );

    if (window == nullptr)
    {
        std::cerr << "No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "No se pudo iniciar GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    lampShader.Use();

    glUniform1i(
        glGetUniformLocation(lampShader.Program, "ourTexture"),
        0
    );

    /*
        Cada vértice tiene 8 valores:

        X, Y, Z,   R, G, B,   U, V

        Las coordenadas UV se conservan como las acomodaste.
    */

    GLfloat vertices[] =
    {
        // ================= CARA FRONTAL =================
        // Posición              Color     U         V
        -0.5f, -0.5f,  0.5f,    1, 1, 1,  0.2925f, 0.34f,
         0.5f, -0.5f,  0.5f,    1, 1, 1,  0.51f,   0.34f,
         0.5f,  0.5f,  0.5f,    1, 1, 1,  0.51f,   0.6667f,
        -0.5f,  0.5f,  0.5f,    1, 1, 1,  0.2925f, 0.6667f,

        // ================= CARA DERECHA =================
         0.5f, -0.5f,  0.5f,    1, 1, 1,  0.5050f, 0.340f,
         0.5f, -0.5f, -0.5f,    1, 1, 1,  0.724f,  0.3400f,
         0.5f,  0.5f, -0.5f,    1, 1, 1,  0.724f,  0.6667f,
         0.5f,  0.5f,  0.5f,    1, 1, 1,  0.5050f, 0.6667f,

         // ================= CARA TRASERA =================
          0.5f, -0.5f, -0.5f,    1, 1, 1,  0.72f,  0.34f,
         -0.5f, -0.5f, -0.5f,    1, 1, 1,  0.935f, 0.34f,
         -0.5f,  0.5f, -0.5f,    1, 1, 1,  0.935f, 0.6667f,
          0.5f,  0.5f, -0.5f,    1, 1, 1,  0.72f,  0.6667f,

          // ================= CARA IZQUIERDA =================
          -0.5f, -0.5f, -0.5f,    1, 1, 1,  0.0685f, 0.34f,
          -0.5f, -0.5f,  0.5f,    1, 1, 1,  0.295f,  0.34f,
          -0.5f,  0.5f,  0.5f,    1, 1, 1,  0.295f,  0.6667f,
          -0.5f,  0.5f, -0.5f,    1, 1, 1,  0.0685f, 0.6667f,

          // ================= CARA SUPERIOR =================
          -0.5f,  0.5f,  0.5f,    1, 1, 1,  0.2925f, 0.6667f,
           0.5f,  0.5f,  0.5f,    1, 1, 1,  0.51f,   0.6667f,
           0.5f,  0.5f, -0.5f,    1, 1, 1,  0.51f,   0.9814f,
          -0.5f,  0.5f, -0.5f,    1, 1, 1,  0.2925f, 0.9814f,

          // ================= CARA INFERIOR =================
          -0.5f, -0.5f, -0.5f,    1, 1, 1,  0.2925f, 0.018753f,
           0.5f, -0.5f, -0.5f,    1, 1, 1,  0.51f,   0.018753f,
           0.5f, -0.5f,  0.5f,    1, 1, 1,  0.51f,   0.3475f,
          -0.5f, -0.5f,  0.5f,    1, 1, 1,  0.2925f, 0.3475f
    };

    GLuint indices[] =
    {
         0,  1,  2,   0,  2,  3,
         4,  5,  6,   4,  6,  7,
         8,  9, 10,   8, 10, 11,
        12, 13, 14,  12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    // Posición
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)0
    );
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)(3 * sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);

    // Coordenadas UV
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)(6 * sizeof(GLfloat))
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // ================= TEXTURA DE LOS DADOS =================

    GLuint texture1;
    glGenTextures(1, &texture1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    );
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR
    );

    int textureWidth = 0;
    int textureHeight = 0;
    int nrChannels = 0;

    stbi_set_flip_vertically_on_load(true);

    const char* texturePath = "images/dice_v2.png";

    unsigned char* image = stbi_load(
        texturePath,
        &textureWidth,
        &textureHeight,
        &nrChannels,
        0
    );

    if (image == nullptr)
    {
        std::cerr << "No se pudo cargar " << texturePath
            << ": " << stbi_failure_reason() << std::endl;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteTextures(1, &texture1);

        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    if (nrChannels != 4)
    {
        std::cerr << texturePath
            << " necesita canal alfa (RGBA). Canales: "
            << nrChannels << std::endl;

        stbi_image_free(image);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteTextures(1, &texture1);

        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        textureWidth,
        textureHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    // ================= TEXTURA NEGRA DEL CORDON =================
    // Se crea en memoria; no necesita otra imagen.

    GLuint texturaCordon;
    glGenTextures(1, &texturaCordon);
    glBindTexture(GL_TEXTURE_2D, texturaCordon);

    unsigned char pixelNegro[] = { 0, 0, 0, 255 };

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        1, 1,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixelNegro
    );

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
    );
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
    );

    GLint modelLoc = glGetUniformLocation(
        lampShader.Program, "model"
    );
    GLint viewLoc = glGetUniformLocation(
        lampShader.Program, "view"
    );
    GLint projLoc = glGetUniformLocation(
        lampShader.Program, "projection"
    );

    // ================= CICLO DE DIBUJO =================

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        DoMovement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lampShader.Use();

        glm::mat4 view = camera.GetViewMatrix();

        // Conserva la llamada a perspectiva de tu código.
        glm::mat4 projection = glm::perspective(
            camera.GetZoom(),
            static_cast<GLfloat>(SCREEN_WIDTH) / SCREEN_HEIGHT,
            0.1f,
            100.0f
        );

        glUniformMatrix4fv(
            viewLoc, 1, GL_FALSE, glm::value_ptr(view)
        );
        glUniformMatrix4fv(
            projLoc, 1, GL_FALSE, glm::value_ptr(projection)
        );

        // Restaurar la textura de los dados cada cuadro.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);

        // ================= DADO IZQUIERDO =================

        glm::mat4 model1(1.0f);

        // Posición: X, Y, Z
        model1 = glm::translate(
            model1,
            glm::vec3(-0.65f, 0.15f, 0.0f)
        );

        // Rotación en X
        model1 = glm::rotate(
            model1,
            glm::radians(180.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Rotación en Y
        model1 = glm::rotate(
            model1,
            glm::radians(-20.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Rotación en Z
        model1 = glm::rotate(
            model1,
            glm::radians(-52.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // Tamaño
        model1 = glm::scale(
            model1,
            glm::vec3(0.85f)
        );

        glUniformMatrix4fv(
            modelLoc, 1, GL_FALSE, glm::value_ptr(model1)
        );

        glDrawElements(
            GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0
        );

        // ================= DADO DERECHO =================

        glm::mat4 model2(1.0f);

        // Posición
        model2 = glm::translate(
            model2,
            glm::vec3(0.60f, -0.10f, 0.0f)
        );

        // Rotación en X
        model2 = glm::rotate(
            model2,
            glm::radians(10.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        // Girar hacia el otro lado para mostrar otra cara lateral.
        model2 = glm::rotate(
            model2,
            glm::radians(-55.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Ladearlo en el sentido contrario al giro anterior.
        model2 = glm::rotate(
            model2,
            glm::radians(-25.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        model2 = glm::scale(
            model2,
            glm::vec3(0.85f)
        );

        glUniformMatrix4fv(
            modelLoc, 1, GL_FALSE, glm::value_ptr(model2)
        );

        glDrawElements(
            GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0
        );

        // ================= CORDONCITO NEGRO =================

        // Conexiones ligeramente dentro de las caras superiores.
        // Se transforman con cada dado para seguir su movimiento.
        glm::vec3 inicio = glm::vec3(
            model1 * glm::vec4(0.0f, 0.48f, 0.0f, 1.0f)
        );

        glm::vec3 fin = glm::vec3(
            model2 * glm::vec4(0.0f, 0.48f, 0.0f, 1.0f)
        );

        // Dos puntos de control:
        // el primero levanta el arco;
        // el segundo crea un doblez antes de llegar al dado derecho.

        // Punto alto donde se unen las dos curvas.
        // Aumenta 1.10f si quieres elevar todavía más el cordón.
       // Unión entre las dos curvas.
        // La bajamos para crear un doblez entre ambos arcos.
        glm::vec3 puntoAlto = (inicio + fin) * 0.5f;
        puntoAlto.y += 0.55f;

        // Primera curva: sube bastante y después baja.
        glm::vec3 controlA = inicio + glm::vec3(
            0.60f, 1.80f, 0.0f
        );

        // Hace que la curva pase por un pequeño valle
        // antes de alcanzar la unión.
        glm::vec3 controlB = puntoAlto + glm::vec3(
            -0.30f, -0.35f, 0.0f
        );

        // Segunda curva: vuelve a subir.
        // Mantiene una unión suave con la primera.
        glm::vec3 controlC = puntoAlto + glm::vec3(
            0.30f, 0.35f, 0.0f
        );

        // Forma el segundo arco antes de bajar al dado derecho.
        glm::vec3 controlD = fin + glm::vec3(
            -0.10f, 1.10f, 0.0f
        );

        // Evaluar una curva de Bézier cúbica.
        auto bezier = [](
            float t,
            const glm::vec3& p0,
            const glm::vec3& p1,
            const glm::vec3& p2,
            const glm::vec3& p3
            ) -> glm::vec3
            {
                float u = 1.0f - t;

                return
                    u * u * u * p0 +
                    3.0f * u * u * t * p1 +
                    3.0f * u * t * t * p2 +
                    t * t * t * p3;
            };

        // Recorrer las dos curvas como un solo cordón.
        // La unión en puntoAlto es suave.
        auto puntoCordon = [&](float t) -> glm::vec3
            {
                if (t <= 0.5f)
                {
                    return bezier(
                        t * 2.0f,
                        inicio,
                        controlA,
                        controlB,
                        puntoAlto
                    );
                }

                return bezier(
                    (t - 0.5f) * 2.0f,
                    puntoAlto,
                    controlC,
                    controlD,
                    fin
                );
            };

        glBindTexture(GL_TEXTURE_2D, texturaCordon);

        // Reutilizar el cubo como segmentos muy delgados.
        for (int i = 0; i < SEGMENTOS_CORDON; ++i)
        {
            float t0 =
                static_cast<float>(i) / SEGMENTOS_CORDON;

            float t1 =
                static_cast<float>(i + 1) / SEGMENTOS_CORDON;

            glm::vec3 a = puntoCordon(t0);
            glm::vec3 b = puntoCordon(t1);

            glm::vec3 diferencia = b - a;
            float longitud = glm::length(diferencia);

            if (longitud < 0.00001f)
                continue;

            // Dirección del segmento.
            glm::vec3 ejeY = diferencia / longitud;

            // Eje auxiliar para construir su orientación.
            glm::vec3 auxiliar(0.0f, 0.0f, 1.0f);

            if (glm::abs(glm::dot(ejeY, auxiliar)) > 0.95f)
            {
                auxiliar = glm::vec3(1.0f, 0.0f, 0.0f);
            }

            glm::vec3 ejeX = glm::normalize(
                glm::cross(ejeY, auxiliar)
            );

            glm::vec3 ejeZ = glm::normalize(
                glm::cross(ejeX, ejeY)
            );

            glm::mat4 orientacion(1.0f);

            orientacion[0] = glm::vec4(ejeX, 0.0f);
            orientacion[1] = glm::vec4(ejeY, 0.0f);
            orientacion[2] = glm::vec4(ejeZ, 0.0f);

            // Colocar el segmento en el punto medio.
            glm::mat4 modelCordon = glm::translate(
                glm::mat4(1.0f),
                (a + b) * 0.5f
            );

            // Orientarlo a lo largo de la curva.
            modelCordon = modelCordon * orientacion;

            // Darle grosor y longitud.
            // Los segmentos se solapan para evitar huecos.
            modelCordon = glm::scale(
                modelCordon,
                glm::vec3(
                    GROSOR_CORDON,
                    longitud + GROSOR_CORDON,
                    GROSOR_CORDON
                )
            );

            glUniformMatrix4fv(
                modelLoc,
                1,
                GL_FALSE,
                glm::value_ptr(modelCordon)
            );

            glDrawElements(
                GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0
            );
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    // ================= LIBERAR RECURSOS =================

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texturaCordon);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void KeyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mode
)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = static_cast<GLfloat>(xPos);
        lastY = static_cast<GLfloat>(yPos);
        firstMouse = false;
    }

    GLfloat xOffset = static_cast<GLfloat>(xPos) - lastX;
    GLfloat yOffset = lastY - static_cast<GLfloat>(yPos);

    lastX = static_cast<GLfloat>(xPos);
    lastY = static_cast<GLfloat>(yPos);

    camera.ProcessMouseMovement(xOffset, yOffset);
}