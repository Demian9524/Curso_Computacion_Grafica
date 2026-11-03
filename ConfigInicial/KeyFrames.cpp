// Practica #11
// Bello Zaragoza Demian
// Fecha de entrega: 31 de octubre de 2026
// Número de cuenta: 320200928
// P inicia/pausa/reanuda. F1 muestra todos los controles en la consola.


#include <iostream>
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>
#include <cstdlib>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"


// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void FramebufferCallback(GLFWwindow*, int, int);
void FocusCallback(GLFWwindow*, int);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,2.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 0.0f),
    glm::vec3(0.0f,0.0f,  0.0f),
    glm::vec3(0.0f,0.0f, 0.0f)
};

glm::vec3 Light1(0.0f);

// Cada canal es independiente y se guarda en TODOS los keyframes.
enum Canal {
    POS_X, POS_Y, POS_Z, GIRO_CUERPO, INCLINACION_CUERPO,
    CABEZA_Z, CABEZA_X, CABEZA_Y,
    PATA_DI_X, PATA_DD_X, PATA_TI_X, PATA_TD_X,
    PATA_DI_Z, PATA_DD_Z, PATA_TI_Z, PATA_TD_Z,
    COLA_Z, COLA_Y, NUM_CANALES
};
struct Pose { std::array<float, NUM_CANALES> v{}; };
struct FRAME {
    Pose pose;
    float segundos = 0.8f; // Duracion para llegar A ESTE fotograma.
};
const int MAX_FRAMES = 200;
std::vector<FRAME> KeyFrame;
Pose poseActual, origenTramo;
bool play = false, pausa = false, iniciada = false;
size_t destino = 0;
float tiempoTramo = 0.0f, duracionManual = 0.8f;
int parteSeleccionada = 0;
bool girarPelota = false;
float rotBall = 0.0f;
const char* ARCHIVO_POSES = "Previo12_Keyframes.txt";
const char* nombresPartes[] = {
    "cuerpo", "cabeza", "pata delantera izquierda", "pata delantera derecha",
    "pata trasera izquierda", "pata trasera derecha", "cola"
};

float Limitar(float x, float a, float b) { return (std::max)(a, (std::min)(b, x)); }
float DiferenciaAngular(float a, float b) {
    float d = std::fmod(b - a, 360.0f);
    if (d > 180.0f) d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    return d;
}
Pose Mezclar(const Pose& a, const Pose& b, float t) {
    // Aceleracion y frenado suaves, sin sumar errores de un cuadro a otro.
    t = Limitar(t, 0.0f, 1.0f);
    float u = t * t * (3.0f - 2.0f * t);
    Pose p;
    for (int c = 0; c < NUM_CANALES; ++c) {
        float diferencia = c >= GIRO_CUERPO ? DiferenciaAngular(a.v[c], b.v[c])
            : b.v[c] - a.v[c];
        p.v[c] = a.v[c] + diferencia * u;
    }
    return p;
}
void Detener() {
    play = pausa = iniciada = false;
    destino = 0; tiempoTramo = 0.0f;
}
void AgregarPose(const Pose& p, float segundos) {
    if (KeyFrame.size() >= MAX_FRAMES) return;
    FRAME f; f.pose = p; f.segundos = Limitar(segundos, 0.1f, 10.0f);
    KeyFrame.push_back(f);
}
void CargarSaludo() {
    Detener(); KeyFrame.clear(); poseActual = Pose{};
    Pose p;
    AgregarPose(p, 0.6f); // De pie.
    p.v[INCLINACION_CUERPO] = -10.0f;
    p.v[PATA_DI_X] = p.v[PATA_DD_X] = 10.0f;
    p.v[PATA_TI_X] = p.v[PATA_TD_X] = -20.0f;
    AgregarPose(p, 0.55f); // Preparacion.

    // Sentarse: las patas traseras se recogen HACIA +Z (el frente del perro).
    // Una pata que parte hacia -Y necesita giro NEGATIVO en X para ir a +Z.
    // El giro anterior (+65) la mandaba atras y mantenia elevada la cadera.
    // Son piezas rigidas; no se inventan rodillas que los OBJ no contienen.
    p.v[INCLINACION_CUERPO] = -35.0f;
    p.v[CABEZA_X] = 28.0f; // Compensa parte de la inclinacion del cuerpo.
    p.v[PATA_DI_X] = p.v[PATA_DD_X] = 35.0f;
    // -35 del cuerpo + -50 de la pata = -85 grados: casi horizontal al frente.
    // Al retirar ese apoyo vertical, MinimoApoyo permite bajar la cadera
    // hasta el apoyo del cuerpo/pata delantera, sin hundir la malla en el piso.
    p.v[PATA_TI_X] = p.v[PATA_TD_X] = -50.0f;
    p.v[COLA_Z] = 10.0f;
    AgregarPose(p, 1.35f);
    AgregarPose(p, 0.55f); // Pausa sentado.

    // Pata DERECHA DEL PERRO. -35 del cuerpo + -45 = -80 grados al frente.
    // Antes +110 daba +75 grados y extendia la pata DETRAS del cuerpo.
    p.v[PATA_DD_X] = -45.0f;
    p.v[PATA_DD_Z] = -10.0f;
    p.v[CABEZA_Z] = -8.0f;
    AgregarPose(p, 0.8f); // Estirar la pata hacia delante.
    for (int i = 0; i < 6; ++i) {
        p.v[PATA_DD_Z] = (i % 2 == 0) ? 12.0f : -12.0f;
        p.v[COLA_Z] = (i % 2 == 0) ? -18.0f : 18.0f;
        AgregarPose(p, 0.32f); // Saludo y cola, ambos por keyframes.
    }
    p.v[PATA_DD_Z] = 0.0f; p.v[COLA_Z] = 0.0f;
    AgregarPose(p, 0.35f);
    p.v[PATA_DD_X] = 35.0f; p.v[CABEZA_Z] = 0.0f;
    AgregarPose(p, 0.8f); // Apoyar de nuevo.
    AgregarPose(p, 0.6f);
    AgregarPose(Pose{}, 1.3f); // Volver a ponerse de pie.
    AgregarPose(Pose{}, 0.5f);
    std::cout << "Saludo listo: " << KeyFrame.size() << " poses. Pulsa P o L.\n";
}
void saveFrame() {
    if (iniciada) { std::cout << "Pulsa R antes de editar/grabar poses.\n"; return; }
    if (KeyFrame.size() >= MAX_FRAMES) { std::cout << "Limite de poses alcanzado.\n"; return; }
    AgregarPose(poseActual, duracionManual);
    std::cout << "Pose " << KeyFrame.size() << " guardada; duracion " << duracionManual << " s.\n";
}
void Comenzar() {
    if (KeyFrame.size() < 2) { std::cout << "Necesitas al menos dos poses (K).\n"; return; }
    // Transicion desde la pose visible al primer keyframe: nunca teletransportar.
    origenTramo = poseActual; destino = 0; tiempoTramo = 0.0f;
    play = iniciada = true; pausa = false;
}
void ActualizarKeyframes(float dt) {
    if (!play || pausa || dt <= 0.0f || !std::isfinite(dt)) return;
    while (play && dt > 0.0f) {
        const FRAME& f = KeyFrame[destino];
        float paso = (std::min)(dt, f.segundos - tiempoTramo);
        tiempoTramo += paso; dt -= paso;
        poseActual = Mezclar(origenTramo, f.pose, tiempoTramo / f.segundos);
        if (tiempoTramo + 0.000001f >= f.segundos) {
            poseActual = f.pose; origenTramo = poseActual;
            tiempoTramo = 0.0f;
            if (++destino >= KeyFrame.size()) {
                Detener(); std::cout << "Animacion terminada. P/L para repetir.\n";
            }
        }
    }
}
bool ExportarPoses() {
    if (KeyFrame.size() < 2) { std::cout << "Graba al menos dos poses antes de exportar.\n"; return false; }
    std::ofstream f(ARCHIVO_POSES);
    if (!f) { std::cerr << "No se pudo escribir " << ARCHIVO_POSES << '\n'; return false; }
    f << "PREVIO12_KEYFRAMES_V1\n" << KeyFrame.size() << '\n' << std::setprecision(9);
    for (const FRAME& k : KeyFrame) {
        f << k.segundos;
        for (float x : k.pose.v) f << ' ' << x;
        f << '\n';
    }
    if (!f) { std::cerr << "Error al guardar las poses.\n"; return false; }
    std::cout << "Guardado: " << ARCHIVO_POSES << " (directorio de trabajo).\n";
    return true;
}
bool ImportarPoses() {
    std::ifstream f(ARCHIVO_POSES);
    std::string firma; int cantidad = 0;
    if (!(f >> firma >> cantidad) || firma != "PREVIO12_KEYFRAMES_V1" || cantidad < 2 || cantidad > MAX_FRAMES) {
        std::cerr << "Archivo de poses inexistente o invalido.\n"; return false;
    }
    std::vector<FRAME> nuevas;
    for (int i = 0; i < cantidad; ++i) {
        FRAME k;
        if (!(f >> k.segundos) || !std::isfinite(k.segundos) || k.segundos < 0.1f || k.segundos > 10.0f) return false;
        for (float& x : k.pose.v)
            if (!(f >> x) || !std::isfinite(x) || std::fabs(x) > 10000.0f) return false;
        nuevas.push_back(k);
    }
    // Solo reemplazar la secuencia cuando TODO el archivo sea valido.
    Detener(); KeyFrame.swap(nuevas);
    std::cout << "Poses cargadas. P/L reproduce desde la postura actual.\n";
    return true;
}
// END_KEYFRAME_CORE

// Transformaciones del modelo dividido; se mantienen los montajes originales.
enum Pieza { CUERPO, CABEZA, COLA, DEL_IZQ, DEL_DER, TRA_IZQ, TRA_DER, NUM_PIEZAS };
const char* rutasPerro[NUM_PIEZAS] = {
    "Models/DogBody.obj", "Models/HeadDog.obj", "Models/TailDog.obj",
    "Models/F_LeftLegDog.obj", "Models/F_RightLegDog.obj",
    "Models/B_LeftLegDog.obj", "Models/B_RightLegDog.obj"
};
const glm::vec3 montajes[NUM_PIEZAS] = {
    glm::vec3(0.0f), glm::vec3(0.0f, 0.093f, 0.208f),
    glm::vec3(0.0f, 0.026f, -0.288f), glm::vec3(0.112f, -0.044f, 0.074f),
    glm::vec3(-0.111f, -0.055f, 0.074f), glm::vec3(0.082f, -0.046f, -0.218f),
    glm::vec3(-0.083f, -0.057f, -0.231f)
};
std::vector<glm::vec3> verticesApoyo[NUM_PIEZAS];
float minimoReposo = 0.0f;
bool ajustarApoyo = false;
glm::mat4 MatrizPieza(const Pose& p, int pieza) {
    glm::mat4 m(1.0f);
    const glm::vec3 cadera(0.0f, -0.05f, -0.22f);
    m = glm::translate(m, cadera);
    m = glm::rotate(m, glm::radians(p.v[INCLINACION_CUERPO]), glm::vec3(1, 0, 0));
    m = glm::translate(m, -cadera);
    m = glm::translate(m, montajes[pieza]);
    if (pieza == CABEZA) {
        m = glm::rotate(m, glm::radians(p.v[CABEZA_Y]), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(p.v[CABEZA_X]), glm::vec3(1, 0, 0));
        m = glm::rotate(m, glm::radians(p.v[CABEZA_Z]), glm::vec3(0, 0, 1));
    }
    else if (pieza == COLA) {
        m = glm::rotate(m, glm::radians(p.v[COLA_Y]), glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::radians(p.v[COLA_Z]), glm::vec3(0, 0, -1));
    }
    else if (pieza >= DEL_IZQ) {
        const int canalesX[] = { PATA_DI_X, PATA_DD_X, PATA_TI_X, PATA_TD_X };
        const int canalesZ[] = { PATA_DI_Z, PATA_DD_Z, PATA_TI_Z, PATA_TD_Z };
        int i = pieza - DEL_IZQ;
        m = glm::rotate(m, glm::radians(p.v[canalesZ[i]]), glm::vec3(0, 0, 1));
        m = glm::rotate(m, glm::radians(p.v[canalesX[i]]), glm::vec3(1, 0, 0));
    }
    return m;
}
float MinimoApoyo(const Pose& p) {
    float minimo = (std::numeric_limits<float>::max)();
    // Cuerpo y cuatro patas: calculo con los vertices reales, no con una caja grande.
    for (int i = 0; i < NUM_PIEZAS; ++i) {
        if (i == CABEZA || i == COLA) continue;
        glm::mat4 m = MatrizPieza(p, i);
        for (const glm::vec3& v : verticesApoyo[i])
            minimo = (std::min)(minimo, (m * glm::vec4(v, 1.0f)).y);
    }
    return minimo;
}
void PrepararApoyo() {
    // Conserva el nivel de apoyo del modelo original al sentarse, sin editar los OBJ.
    // POS_Y sigue disponible como desplazamiento manual adicional.
    ajustarApoyo = true;
    for (int i = 0; i < NUM_PIEZAS; ++i) {
        if (i == CABEZA || i == COLA) continue;
        std::ifstream archivo(rutasPerro[i]); std::string linea, tipo;
        while (std::getline(archivo, linea)) {
            std::istringstream datos(linea); glm::vec3 v;
            if ((datos >> tipo) && tipo == "v" && (datos >> v.x >> v.y >> v.z))
                verticesApoyo[i].push_back(v);
        }
        if (verticesApoyo[i].empty()) ajustarApoyo = false;
    }
    if (ajustarApoyo) minimoReposo = MinimoApoyo(Pose{});
    else std::cerr << "No se pudo medir el apoyo. Revisa Models y el directorio de trabajo.\n";
}
void Ayuda() {
    std::cout << "\nPREVIO 12 - Bello Zaragoza Demian - 320200928\n"
        "P: iniciar/pausar/reanudar | L: reproducir desde la pose visible\n"
        "F2: preparar saludo | R: detener y volver a la primera pose\n"
        "F3: vaciar secuencia para grabar | K: guardar pose actual\n"
        "F5: exportar poses TXT | F9: cargar poses TXT\n"
        "TAB: elegir cuerpo/cabeza/4 patas/cola | Z/X: angulo principal\n"
        "C/V: angulo secundario | 2/3: giro cuerpo | 4/5: cabeza\n"
        "G/J: posicion X | Y/H: posicion Z | T/U: altura del perro\n"
        "Menos/Igual: duracion del siguiente keyframe (0.1 a 10 s)\n"
        "WASD/flechas y mouse: camara | M: liberar/capturar mouse\n"
        "N: giro pelota | Espacio: luz variable | F1: ayuda | Esc: salir\n"
        "F5 guarda las poses; el video MP4 se graba con una captura de pantalla externa.\n\n";
}


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
    // Init GLFW
    if (!glfwInit()) return EXIT_FAILURE;
    // Set all the required options for GLFW
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Previo practica #12 - Bello Zaragoza Demian - 320200928", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, FramebufferCallback);
    glfwSetWindowFocusCallback(window, FocusCallback);
    glfwSwapInterval(1);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");



    //models
    Model DogBody((char*)"Models/DogBody.obj");
    Model HeadDog((char*)"Models/HeadDog.obj");
    Model DogTail((char*)"Models/TailDog.obj");
    Model F_RightLeg((char*)"Models/F_RightLegDog.obj");
    Model F_LeftLeg((char*)"Models/F_LeftLegDog.obj");
    Model B_RightLeg((char*)"Models/B_RightLegDog.obj");
    Model B_LeftLeg((char*)"Models/B_LeftLegDog.obj");
    Model Piso((char*)"Models/piso.obj");
    Model Ball((char*)"Models/ball.obj");


    PrepararApoyo();
    CargarSaludo();
    Ayuda();

    // Set texture units
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);



    lastFrame = static_cast<float>(glfwGetTime());

    // Game loop
    while (!glfwWindowShouldClose(window))
    {

        // Calculate deltatime of current frame
        GLfloat currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = Limitar(currentFrame - lastFrame, 0.0f, 0.1f);
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        DoMovement();
        if (SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0) continue;
        Animation();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT, 0.1f, 100.0f);

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OpenGL options
        glEnable(GL_DEPTH_TEST);






        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();

        glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
        //glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


        // Directional light
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


        // Point light 1
        glm::vec3 lightColor;
        lightColor.x = static_cast<float>(std::fabs(std::sin(glfwGetTime() * Light1.x)));
        lightColor.y = static_cast<float>(std::fabs(std::sin(glfwGetTime() * Light1.y)));
        lightColor.z = static_cast<float>(std::fabs(std::sin(glfwGetTime() * Light1.z)));


        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);


        // SpotLight
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));


        // Set material properties
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


        glm::mat4 model(1);



        // Piso: conservar su transformacion original.
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        view = camera.GetViewMatrix();
        model = glm::mat4(1);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Piso.Draw(lightingShader);

        model = glm::mat4(1);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        // Una matriz por pieza, todos los canales proceden del keyframe interpolado.
        float apoyoY = ajustarApoyo ? minimoReposo - MinimoApoyo(poseActual) : 0.0f;
        glm::mat4 raiz = glm::translate(glm::mat4(1.0f), glm::vec3(
            poseActual.v[POS_X], poseActual.v[POS_Y] + apoyoY, poseActual.v[POS_Z]));
        raiz = glm::rotate(raiz, glm::radians(poseActual.v[GIRO_CUERPO]), glm::vec3(0, 1, 0));
        Model* piezas[NUM_PIEZAS] = {
            &DogBody, &HeadDog, &DogTail, &F_LeftLeg, &F_RightLeg, &B_LeftLeg, &B_RightLeg
        };
        for (int i = 0; i < NUM_PIEZAS; ++i) {
            model = raiz * MatrizPieza(poseActual, i);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            piezas[i]->Draw(lightingShader);
        }

        model = glm::mat4(1);
        glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
        model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Ball.Draw(lightingShader);
        glDisable(GL_BLEND);  //Desactiva el canal alfa 
        glBindVertexArray(0);


        // La iluminacion permanece; no se dibuja el cubo blanco de la lampara.

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }




    // Liberar la ventana al terminar.
    glfwDestroyWindow(window);
    glfwTerminate();



    return 0;
}

void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);
    // Evita que las teclas manuales sobrescriban una animacion incluso estando pausada.
    if (iniciada) return;
    const float giro = 55.0f * deltaTime;
    const float paso = 0.35f * deltaTime;
    poseActual.v[POS_X] += (int(keys[GLFW_KEY_J]) - int(keys[GLFW_KEY_G])) * paso;
    poseActual.v[POS_Z] += (int(keys[GLFW_KEY_H]) - int(keys[GLFW_KEY_Y])) * paso;
    poseActual.v[POS_Y] += (int(keys[GLFW_KEY_T]) - int(keys[GLFW_KEY_U])) * paso;
    poseActual.v[GIRO_CUERPO] += (int(keys[GLFW_KEY_2]) - int(keys[GLFW_KEY_3])) * giro;
    poseActual.v[CABEZA_Z] += (int(keys[GLFW_KEY_4]) - int(keys[GLFW_KEY_5])) * giro;
    const int primario[] = { INCLINACION_CUERPO, CABEZA_Z, PATA_DI_X, PATA_DD_X,
                            PATA_TI_X, PATA_TD_X, COLA_Z };
    const int secundario[] = { GIRO_CUERPO, CABEZA_Y, PATA_DI_Z, PATA_DD_Z,
                              PATA_TI_Z, PATA_TD_Z, COLA_Y };
    poseActual.v[primario[parteSeleccionada]] += (int(keys[GLFW_KEY_X]) - int(keys[GLFW_KEY_Z])) * giro;
    poseActual.v[secundario[parteSeleccionada]] += (int(keys[GLFW_KEY_V]) - int(keys[GLFW_KEY_C])) * giro;
}

void KeyCallback(GLFWwindow* window, int key, int, int action, int)
{
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) keys[key] = true;
        if (action == GLFW_RELEASE) keys[key] = false;
    }
    // Cada pulsacion realiza UNA accion; soltar/repetir no guarda poses adicionales.
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
    case GLFW_KEY_P:
        if (!iniciada) Comenzar();
        else { pausa = !pausa; std::cout << (pausa ? "Pausa.\n" : "Reanudado.\n"); }
        break;
    case GLFW_KEY_L: Comenzar(); break;
    case GLFW_KEY_K: saveFrame(); break;
    case GLFW_KEY_R:
        Detener(); poseActual = KeyFrame.empty() ? Pose{} : KeyFrame.front().pose; break;
    case GLFW_KEY_F1: Ayuda(); break;
    case GLFW_KEY_F2: CargarSaludo(); break;
    case GLFW_KEY_F3:
        Detener(); KeyFrame.clear(); poseActual = Pose{};
        std::cout << "Secuencia vacia. K guarda cada pose; F2 recupera el saludo.\n"; break;
    case GLFW_KEY_F5: ExportarPoses(); break;
    case GLFW_KEY_F9:
        if (!ImportarPoses()) std::cerr << "No se cambio la secuencia actual.\n";
        break;
    case GLFW_KEY_TAB:
        parteSeleccionada = (parteSeleccionada + 1) % 7;
        std::cout << "Seleccion: " << nombresPartes[parteSeleccionada] << '\n'; break;
    case GLFW_KEY_MINUS:
        duracionManual = Limitar(duracionManual - 0.1f, 0.1f, 10.0f);
        std::cout << "Duracion: " << duracionManual << " s\n"; break;
    case GLFW_KEY_EQUAL:
        duracionManual = Limitar(duracionManual + 0.1f, 0.1f, 10.0f);
        std::cout << "Duracion: " << duracionManual << " s\n"; break;
    case GLFW_KEY_N: girarPelota = !girarPelota; break;
    case GLFW_KEY_M:
        glfwSetInputMode(window, GLFW_CURSOR,
            glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        firstMouse = true; break;
    case GLFW_KEY_SPACE:
        active = !active;
        Light1 = active ? glm::vec3(0.2f, 0.8f, 1.0f) : glm::vec3(0.0f);
        break;
    }
}
void Animation()
{
    ActualizarKeyframes(deltaTime);
    if (girarPelota) rotBall = std::fmod(rotBall + 35.0f * deltaTime, 360.0f);
}
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) { firstMouse = true; return; }
    if (firstMouse) {
        lastX = static_cast<float>(xPos); lastY = static_cast<float>(yPos); firstMouse = false;
    }
    float dx = static_cast<float>(xPos) - lastX;
    float dy = lastY - static_cast<float>(yPos);
    lastX = static_cast<float>(xPos); lastY = static_cast<float>(yPos);
    camera.ProcessMouseMovement(dx, dy);
}
void FramebufferCallback(GLFWwindow*, int w, int h)
{
    SCREEN_WIDTH = w; SCREEN_HEIGHT = h; glViewport(0, 0, w, h);
}
void FocusCallback(GLFWwindow*, int focused)
{
    firstMouse = true;
    if (!focused) {
        std::fill(keys, keys + 1024, false);
        if (iniciada) pausa = true;
    }
    lastFrame = static_cast<float>(glfwGetTime());
}
