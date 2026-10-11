// Práctica #8
// Bello Zaragoza Demian
// Fecha de entrega: 10 octubre 2026
// Número de cuenta: 320200928

// Ciclo de dia y noche
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <limits>
#include <locale>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Camera.h"

#define USAR_PERRITO 1
#define USAR_ARBOL 1
#if USAR_PERRITO || USAR_ARBOL
#include "Model.h"
#include "stb_image.h"
#endif

const GLuint WIDTH = 1100, HEIGHT = 720;
const float PI = 3.14159265358979323846f;
float DURACION_CICLO = 45.0f; // Duracion en segundos.
double angle = 0.65;
bool paused = false;
Camera camera(glm::vec3(0.0f, 5.0f, 24.0f));
bool keys[1024] = {};
bool firstMouse = true;
bool mouseCaptured = true;
bool invertirTexturaPerro = false;
bool invertirTexturaArbol = false;
float FACTOR_RADIO = 0.72f;
double lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0;
float SENSIBILIDAD_MOUSE = 0.65f;
const float ALTURA_PISO = 0.0f;
const float ESCALA_PERRITO = 3.0f;
const float DISTANCIA_DETRAS = 0.45f;
const float ESPACIO_SOBRE_CABEZA = 1.2f;
const float TAMANO_SOL = 2.2f;
const float TAMANO_LUNA = 2.0f;

// Modelos y texturas
const char* RUTA_MODELO_PERRO = "Models/RedDog.obj";
const char* RUTA_TEXTURA_PERRO = "Models/Texture_albedo.jpg";
const char* RUTA_MODELO_ARBOL = "Models/MinecraftTree/Mineways2Skfb.obj";
const char* RUTA_TEXTURA_ARBOL = "Models/MinecraftTree/Mineways2Skfb-RGBA.png";
float deltaTime = 0.0f;

void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseCallback(GLFWwindow*, double, double);
void FocusCallback(GLFWwindow*, int);
void MouseButtonCallback(GLFWwindow*, int, int, int);
void CaptureMouse(GLFWwindow*, bool);
void DoMovement();

struct ObjBounds {
    float minX, minY, minZ, maxX, maxY, maxZ;
};

// Obtiene los limites del modelo a partir de sus vertices.
bool ReadObjBounds(const char* path, ObjBounds& bounds) {
    std::ifstream file(path);
    std::string line;
    bounds.minX = bounds.minY = bounds.minZ = std::numeric_limits<float>::max();
    bounds.maxX = bounds.maxY = bounds.maxZ = std::numeric_limits<float>::lowest();
    bool found = false;
    while (std::getline(file, line)) {
        std::istringstream row(line);
        row.imbue(std::locale::classic());
        std::string kind;
        float x, y, z;
        if (row >> kind && kind == "v" && row >> x >> y >> z) {
            if (std::isfinite(x) && std::isfinite(y) && std::isfinite(z)) {
                bounds.minX = std::min(bounds.minX, x);
                bounds.minY = std::min(bounds.minY, y);
                bounds.minZ = std::min(bounds.minZ, z);
                bounds.maxX = std::max(bounds.maxX, x);
                bounds.maxY = std::max(bounds.maxY, y);
                bounds.maxZ = std::max(bounds.maxZ, z);
                found = true;
            }
        }
    }
    return found;
}

bool CalculateModelFloor(const char* path, float scale, float& offset,
    float* height = nullptr, float* backZ = nullptr,
    float* centerX = nullptr) {
    ObjBounds bounds;
    if (scale <= 0.0f || !ReadObjBounds(path, bounds)) return false;
    // Coloca la base del modelo sobre el piso.
    offset = ALTURA_PISO - bounds.minY * scale;
    if (height) *height = (bounds.maxY - bounds.minY) * scale;
    if (backZ) *backZ = bounds.minZ * scale;
    if (centerX) *centerX = (bounds.minX + bounds.maxX) * 0.5f * scale;
    return true;
}

float Smooth(float a, float b, float x) {
    float t = glm::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}
void Vec3(GLuint p, const char* name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(p, name), 1, glm::value_ptr(value));
}
void Mat4(GLuint p, const char* name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(p, name), 1, GL_FALSE, glm::value_ptr(value));
}
void Float(GLuint p, const char* name, float value) {
    glUniform1f(glGetUniformLocation(p, name), value);
}
void Int(GLuint p, const char* name, int value) {
    glUniform1i(glGetUniformLocation(p, name), value);
}
bool Linked(GLuint program) {
    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[4096] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Error de shader: " << log << std::endl;
    }
    return ok == GL_TRUE;
}
void DrawCube(GLuint program, GLuint vao, glm::vec3 position,
    glm::vec3 size, glm::vec3 color, bool ground = false) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    Mat4(program, "model", model);
    Vec3(program, "objectColor", color);
    Int(program, "useTexture", 0);
    Int(program, "isGround", ground ? 1 : 0);
    Float(program, "alphaCutoff", 0.0f);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main() {
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT,
        "Practica 8 Bello Zaragoza Demian", nullptr, nullptr);
    if (!window) {
        std::cerr << "No se pudo crear la ventana OpenGL 3.3.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "No se pudo iniciar GLEW.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetWindowFocusCallback(window, FocusCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    CaptureMouse(window, true);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    Shader sceneShader("Shader/ciclo.vs", "Shader/ciclo.frag");
    Shader astroShader("Shader/astro.vs", "Shader/astro.frag");
    if (!Linked(sceneShader.Program) || !Linked(astroShader.Program)) {
        std::cerr << "Revisa la carpeta Shader y el directorio de trabajo.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Vertices y normales del cubo usado para el piso.
    float vertices[] = {
      -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Cuadrado para dibujar el sol y la luna.
    const float quad[] = {
        -0.5f,-0.5f,0, 0,0,  0.5f,-0.5f,0, 1,0,  0.5f,0.5f,0, 1,1,
        -0.5f,-0.5f,0, 0,0,  0.5f,0.5f,0, 1,1, -0.5f,0.5f,0, 0,1
    };
    GLuint astroVAO, astroVBO;
    glGenVertexArrays(1, &astroVAO);
    glGenBuffers(1, &astroVBO);
    glBindVertexArray(astroVAO);
    glBindBuffer(GL_ARRAY_BUFFER, astroVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    float altoPerrito = 6.0f;
    float fondoPerritoZ = -2.0f;
    float centroPerritoX = 0.0f;
#if USAR_PERRITO

    if (!std::ifstream(RUTA_MODELO_PERRO).good()) {
        std::cerr << "Falta Models/RedDog.obj. Conserva tu carpeta Models,\n"
            "o cambia USAR_PERRITO a 0 para ejecutar sin el perro.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    stbi_set_flip_vertically_on_load(false);
    Model red_dog((char*)RUTA_MODELO_PERRO);
    float alturaPerrito = 0.0f;
    if (!CalculateModelFloor(RUTA_MODELO_PERRO, ESCALA_PERRITO, alturaPerrito,
        &altoPerrito, &fondoPerritoZ, &centroPerritoX)) {
        std::cerr << "No se pudo calcular el punto inferior de RedDog.obj.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::cout << "Altura automatica del perro: " << alturaPerrito << std::endl;
    GLuint dogTexture;
    glGenTextures(1, &dogTexture);

    // Unidad de textura reservada para el albedo.
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, dogTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(false);
    int tw = 0, th = 0, channels = 0;
    unsigned char* data = stbi_load(RUTA_TEXTURA_PERRO, &tw, &th, &channels, 4);
    if (data) {
        std::cout << "Textura cargada: " << RUTA_TEXTURA_PERRO
            << " (" << tw << " x " << th << ")" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else {
        std::cerr << "No se encontro Texture_albedo.jpg; se usara color neutro.\n";
        const unsigned char fallback[] = { 210,180,150,255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallback);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
#endif

#if USAR_ARBOL
    std::unique_ptr<Model> minecraftTree;
    GLuint treeTexture = 0;
    glm::mat4 treeModel(1.0f);
    ObjBounds treeBounds;
    if (ReadObjBounds(RUTA_MODELO_ARBOL, treeBounds)
        && treeBounds.maxY - treeBounds.minY > 0.0001f) {
        stbi_set_flip_vertically_on_load(false);
        int treeWidth = 0, treeHeight = 0, treeChannels = 0;
        unsigned char* treePixels = stbi_load(RUTA_TEXTURA_ARBOL,
            &treeWidth, &treeHeight, &treeChannels, 4);
        if (treePixels) {
            minecraftTree.reset(new Model((char*)RUTA_MODELO_ARBOL));
            glGenTextures(1, &treeTexture);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, treeTexture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, treeWidth, treeHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, treePixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(treePixels);
            glActiveTexture(GL_TEXTURE0);

            // Ajusta el arbol al suelo, detras del perro.
            const float alturaArbol = std::max(4.5f, altoPerrito * 1.8f);
            const float escalaArbol = alturaArbol / (treeBounds.maxY - treeBounds.minY);
            const float centroArbolX = centroPerritoX + altoPerrito * 0.65f;
            const float separacionArbol = std::max(1.0f, altoPerrito * 0.35f);
            glm::vec3 posicionArbol(
                centroArbolX - (treeBounds.minX + treeBounds.maxX) * 0.5f * escalaArbol,
                ALTURA_PISO - treeBounds.minY * escalaArbol,
                fondoPerritoZ - separacionArbol - treeBounds.maxZ * escalaArbol);
            treeModel = glm::translate(glm::mat4(1.0f), posicionArbol);
            treeModel = glm::scale(treeModel, glm::vec3(escalaArbol));
            std::cout << "Arbol cargado: " << RUTA_MODELO_ARBOL
                << " | altura=" << alturaArbol << std::endl;
        }
        else {
            std::cerr << "No se pudo cargar " << RUTA_TEXTURA_ARBOL
                << ". Se continua sin arbol.\n";
        }
    }
    else {
        std::cerr << "No se pudo leer " << RUTA_MODELO_ARBOL
            << ". Se continua sin arbol.\n";
    }
#endif

    // Centro y radios de la orbita.
    const float orbitaZ = fondoPerritoZ - DISTANCIA_DETRAS;
    const float alturaCamara = std::max(5.0f, altoPerrito * 0.65f);
    camera = Camera(glm::vec3(centroPerritoX, alturaCamara, 24.0f));
    const float profundidadOrbita = std::max(1.0f, camera.GetPosition().z - orbitaZ);
    const float mitadVistaY = profundidadOrbita * std::tan(glm::radians(camera.GetZoom()) * 0.5f);
    const float mitadVistaX = mitadVistaY * static_cast<float>(screenWidth) / screenHeight;
    const float centroOrbitaY = ALTURA_PISO + std::max(2.0f, alturaCamara * 0.8f);
    const float radioHorizontalBase = mitadVistaX + TAMANO_SOL * 0.5f + 0.08f;
    const float radioVerticalMinimo = std::max(
        centroOrbitaY + TAMANO_SOL * 0.5f + 0.5f,
        altoPerrito + ESPACIO_SOBRE_CABEZA + TAMANO_SOL * 0.5f - centroOrbitaY);
    const float radioVerticalBase = std::max(
        std::max(centroOrbitaY + TAMANO_SOL * 0.5f + 0.5f,
            altoPerrito + ESPACIO_SOBRE_CABEZA + TAMANO_SOL * 0.5f - centroOrbitaY),
        alturaCamara + mitadVistaY - centroOrbitaY - TAMANO_SOL * 0.5f - 0.4f);
    std::cout << "Orbita cercana: z=" << orbitaZ
        << ", factor de radio=" << FACTOR_RADIO << std::endl;

    std::cout << "ESPACIO: pausa | R: reinicio | 1: sol dominante | 2: luna dominante | 3: mezcla\n"
        "+/-: cambiar duracion | WASD/flechas: mover camara\n"
        "Raton capturado: mirar | F1: liberar/capturar raton\n"
        "Clic izquierdo: capturar | SHIFT: mover mas rapido | ESC: salir\n"
        "T: invertir textura del perro | Y: invertir textura del arbol\n"
        "O: reducir orbita | L: ampliar orbita\n";
    double lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double elapsed = now - lastFrame;
        lastFrame = now;
        deltaTime = static_cast<float>(std::min(elapsed, 0.1));
        glfwPollEvents();
        DoMovement();

        // Una vuelta por ciclo, independiente de los FPS.
        if (!paused)
            angle = std::fmod(angle + (2.0 * PI / DURACION_CICLO) * elapsed, 2.0 * PI);
        float a = static_cast<float>(angle);
        float elevation = std::sin(a);
        const float radioHorizontal = std::max(3.5f, radioHorizontalBase * FACTOR_RADIO);
        const float radioVertical = std::max(radioVerticalMinimo, radioVerticalBase * FACTOR_RADIO);

        // El sol y la luna giran con 180 grados de separacion.
        glm::vec3 sunPos(centroPerritoX + radioHorizontal * std::cos(a), centroOrbitaY + radioVertical * elevation, orbitaZ);
        glm::vec3 moonPos(centroPerritoX - radioHorizontal * std::cos(a), centroOrbitaY - radioVertical * elevation, orbitaZ);

        // Reduce la luz de cada astro al pasar bajo el piso.
        float day = 0.5f + 0.5f * elevation;
        float sunStrength = (0.08f + 1.02f * day)
            * Smooth(-TAMANO_SOL * 0.5f, TAMANO_SOL * 0.5f, sunPos.y - ALTURA_PISO);
        float moonStrength = (0.04f + 0.55f * (1.0f - day))
            * Smooth(-TAMANO_LUNA * 0.5f, TAMANO_LUNA * 0.5f, moonPos.y - ALTURA_PISO);
        float twilight = 1.0f - Smooth(0.0f, 0.28f, std::abs(elevation));

        // Color del cielo durante el ciclo.
        glm::vec3 sky = glm::mix(glm::vec3(0.012f, 0.020f, 0.065f),
            glm::vec3(0.40f, 0.69f, 0.96f), day);
        sky = glm::mix(sky, glm::vec3(0.82f, 0.34f, 0.15f), twilight * 0.65f);
        glClearColor(sky.r, sky.g, sky.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
            static_cast<float>(screenWidth) / screenHeight, 0.1f, 250.0f);
        glm::mat4 view = camera.GetViewMatrix();

        sceneShader.Use();
        GLuint p = sceneShader.Program;
        Mat4(p, "projection", projection);
        Mat4(p, "view", view);
        Vec3(p, "viewPos", camera.GetPosition());

        // Cada luz sigue la posicion de su astro.
        Vec3(p, "sun.position", sunPos);
        Vec3(p, "moon.position", moonPos);
        glm::vec3 sunColor = glm::mix(glm::vec3(1.0f, 0.72f, 0.48f),
            glm::vec3(1.0f, 0.98f, 0.94f), Smooth(0.0f, 0.55f, elevation));
        glm::vec3 moonColor(0.45f, 0.60f, 1.0f);

        Vec3(p, "sun.ambient", glm::vec3(0.24f, 0.23f, 0.20f) * day);
        Vec3(p, "sun.diffuse", sunColor * sunStrength);
        Vec3(p, "sun.specular", sunColor * sunStrength);
        Vec3(p, "moon.ambient", glm::vec3(0.035f, 0.050f, 0.090f) * (1.0f - day));
        Vec3(p, "moon.diffuse", moonColor * moonStrength);
        Vec3(p, "moon.specular", moonColor * moonStrength);

        DrawCube(p, cubeVAO, glm::vec3(0, ALTURA_PISO - 1.0f, 0), glm::vec3(90, 2.0f, 90),
            glm::vec3(1.0f), true);

#if USAR_PERRITO

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0, alturaPerrito, 0));
        model = glm::scale(model, glm::vec3(ESCALA_PERRITO));
        Mat4(p, "model", model);
        Vec3(p, "objectColor", glm::vec3(1.0f));
        Int(p, "useTexture", 1);
        Int(p, "isGround", 0);
        Int(p, "flipDogV", invertirTexturaPerro ? 1 : 0);
        Float(p, "alphaCutoff", 0.0f);
        Int(p, "albedoMap", 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, dogTexture);
        glActiveTexture(GL_TEXTURE0);
        red_dog.Draw(sceneShader);
#endif

#if USAR_ARBOL
        if (minecraftTree) {
            Mat4(p, "model", treeModel);
            Vec3(p, "objectColor", glm::vec3(1.0f));
            Int(p, "useTexture", 1);
            Int(p, "isGround", 0);

            Int(p, "flipDogV", invertirTexturaArbol ? 1 : 0);
            // Recorta las partes transparentes de las hojas.
            Float(p, "alphaCutoff", 0.5f);
            Int(p, "albedoMap", 7);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, treeTexture);
            glActiveTexture(GL_TEXTURE0);
            minecraftTree->Draw(sceneShader);
            Float(p, "alphaCutoff", 0.0f);
        }
#endif

        astroShader.Use();
        GLuint ap = astroShader.Program;
        Mat4(ap, "projection", projection);
        Mat4(ap, "view", view);
        // Orienta los cuadrados hacia la camara.
        glm::mat4 billboard = glm::mat4(glm::transpose(glm::mat3(view)));
        glBindVertexArray(astroVAO);
        for (int i = 0; i < 2; ++i) {
            glm::vec3 position = i == 0 ? sunPos : moonPos;
            float size = i == 0 ? TAMANO_SOL : TAMANO_LUNA;
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            model = model * billboard;
            model = glm::scale(model, glm::vec3(size));
            Mat4(ap, "model", model);
            Int(ap, "isMoon", i);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &astroVAO);
    glDeleteBuffers(1, &astroVBO);
#if USAR_PERRITO
    glDeleteTextures(1, &dogTexture);
#endif
#if USAR_ARBOL
    if (treeTexture) glDeleteTextures(1, &treeTexture);
    minecraftTree.reset();
#endif
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}

void DoMovement() {

    float step = deltaTime * ((keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) ? 3.0f : 1.0f);
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, step);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, step);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, step);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, step);
}
void KeyCallback(GLFWwindow* window, int key, int, int action, int) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        if (action == GLFW_RELEASE) keys[key] = false;
    }
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_F1) CaptureMouse(window, !mouseCaptured);
    if (key == GLFW_KEY_T) {
        invertirTexturaPerro = !invertirTexturaPerro;
        std::cout << "Textura del perro, invertir V: " << (invertirTexturaPerro ? "SI" : "NO") << std::endl;
    }
    if (key == GLFW_KEY_Y) invertirTexturaArbol = !invertirTexturaArbol;
    if (key == GLFW_KEY_O) FACTOR_RADIO = glm::clamp(FACTOR_RADIO - 0.05f, 0.45f, 1.25f);
    if (key == GLFW_KEY_L) FACTOR_RADIO = glm::clamp(FACTOR_RADIO + 0.05f, 0.45f, 1.25f);
    if (key == GLFW_KEY_SPACE) paused = !paused;
    if (key == GLFW_KEY_R) { angle = 0.65; paused = false; }
    if (key == GLFW_KEY_1) { angle = PI * 0.5; paused = true; }
    if (key == GLFW_KEY_2) { angle = PI * 1.5; paused = true; }
    if (key == GLFW_KEY_3) { angle = 0.50; paused = true; }
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        DURACION_CICLO = glm::clamp(DURACION_CICLO - 5.0f, 10.0f, 120.0f);
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        DURACION_CICLO = glm::clamp(DURACION_CICLO + 5.0f, 10.0f, 120.0f);
}
void CaptureMouse(GLFWwindow* window, bool capture) {
    mouseCaptured = capture;
    firstMouse = true;

    glfwSetInputMode(window, GLFW_CURSOR,
        capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

#ifdef GLFW_RAW_MOUSE_MOTION
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, capture ? GLFW_TRUE : GLFW_FALSE);
#endif
}
void FocusCallback(GLFWwindow*, int focused) {
    firstMouse = true; // Evita un salto del raton al volver a la ventana.
    if (!focused)
        for (int i = 0; i < 1024; ++i) keys[i] = false;
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !mouseCaptured)
        CaptureMouse(window, true);
}
void MouseCallback(GLFWwindow* window, double x, double y) {
    if (!mouseCaptured || !glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        firstMouse = true;
        return;
    }
    if (firstMouse) {
        lastX = x; lastY = y; firstMouse = false;
        return;
    }
    float dx = static_cast<float>(x - lastX);
    float dy = static_cast<float>(lastY - y);
    lastX = x; lastY = y;

    // El desplazamiento del raton no necesita deltaTime.
    camera.ProcessMouseMovement(dx * SENSIBILIDAD_MOUSE, dy * SENSIBILIDAD_MOUSE);
}
