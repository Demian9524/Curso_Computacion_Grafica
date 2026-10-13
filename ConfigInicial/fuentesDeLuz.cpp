// Previo practica #9 
// Bello Zaragoza Demian
// Fecha de entrega: 12 de octubre 2026
// Número de cuenta: 320200928

#include <iostream>
#include <fstream>
#include <direct.h>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstddef>

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
void WindowFocusCallback(GLFWwindow* window, int focused);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
bool mouseCaptured = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
float alturaCamara = 0.0f;
bool hojasTransparentes = true;
bool coloresDePrueba = false;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

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



glm::vec3 Light1 = glm::vec3(0);


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


struct VerticeArbol {
	glm::vec3 posicion;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct ParteArbol {
	GLuint vao = 0;
	GLuint vbo = 0;
	GLsizei cantidad = 0;
};

struct ArbolMinecraft {
	ParteArbol tronco;
	ParteArbol hojas;
	GLuint textura = 0;
};

static int IndiceOBJ(const std::string& s, size_t total) {
	if (s.empty()) return -1;
	int n = std::stoi(s);
	return n > 0 ? n - 1 : (n < 0 ? static_cast<int>(total) + n : -1);
}

static void AgregarVertice(const std::string& dato,
	const std::vector<glm::vec3>& posiciones,
	const std::vector<glm::vec2>& uvs,
	const std::vector<glm::vec3>& normales,
	std::vector<VerticeArbol>& destino) {

	size_t a = dato.find('/');
	size_t b = a == std::string::npos ? std::string::npos : dato.find('/', a + 1);
	std::string p = dato.substr(0, a);
	std::string t = a == std::string::npos ? "" : dato.substr(a + 1, b == std::string::npos ? b : b - a - 1);
	std::string n = b == std::string::npos ? "" : dato.substr(b + 1);
	int ip = IndiceOBJ(p, posiciones.size());
	int it = IndiceOBJ(t, uvs.size());
	int in = IndiceOBJ(n, normales.size());
	if (ip < 0 || ip >= static_cast<int>(posiciones.size())) return;
	VerticeArbol vertice;
	vertice.posicion = posiciones[ip];
	vertice.uv = (it >= 0 && it < static_cast<int>(uvs.size())) ? uvs[it] : glm::vec2(0.0f);
	vertice.normal = (in >= 0 && in < static_cast<int>(normales.size())) ? normales[in] : glm::vec3(0.0f, 1.0f, 0.0f);
	destino.push_back(vertice);
}

static void SubirMalla(const std::vector<VerticeArbol>& vertices, ParteArbol& parte) {
	if (vertices.empty()) return;
	parte.cantidad = static_cast<GLsizei>(vertices.size());
	glGenVertexArrays(1, &parte.vao);
	glGenBuffers(1, &parte.vbo);
	glBindVertexArray(parte.vao);
	glBindBuffer(GL_ARRAY_BUFFER, parte.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VerticeArbol), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeArbol), (void*)offsetof(VerticeArbol, posicion));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VerticeArbol), (void*)offsetof(VerticeArbol, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VerticeArbol), (void*)offsetof(VerticeArbol, uv));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

static GLuint CompilarShaderArbol(GLenum tipo, const char* fuente) {
	GLuint shader = glCreateShader(tipo);
	glShaderSource(shader, 1, &fuente, nullptr);
	glCompileShader(shader);
	GLint correcto = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &correcto);
	if (correcto != GL_TRUE) {
		GLint tam = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tam);
		std::vector<GLchar> mensaje(static_cast<size_t>(std::max(tam, 1)));
		glGetShaderInfoLog(shader, static_cast<GLsizei>(mensaje.size()), nullptr, mensaje.data());
		std::cerr << "ERROR en shader del arbol (" << (tipo == GL_VERTEX_SHADER ? "vertice" : "fragmento")
			<< "): " << mensaje.data() << std::endl;
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

static GLuint CrearProgramaArbol() {
	// El arbol tiene su propio renderizador; no modifica lighting.frag ni lighting.vs.
	const char* vertex = R"GLSL(#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 posMundo;
out vec3 normalMundo;
out vec2 uv;
void main() {
    vec4 mundo = model * vec4(aPos, 1.0);
    posMundo = mundo.xyz;
    normalMundo = mat3(transpose(inverse(model))) * aNormal;
    uv = aUV;
    gl_Position = projection * view * mundo;
}
)GLSL";
	const char* fragment = R"GLSL(#version 330 core
in vec3 posMundo;
in vec3 normalMundo;
in vec2 uv;
uniform sampler2D atlas;
uniform vec3 posicionLuz;
uniform int esHoja;
uniform int texturaCorrecta;
uniform float opacidad;
uniform int verColoresPrueba;
out vec4 FragColor;
void main() {
    vec3 respaldo = esHoja == 1 ? vec3(0.24, 0.68, 0.25) : vec3(0.53, 0.32, 0.16);
    vec4 pixel = texturaCorrecta == 1 ? texture(atlas, uv) : vec4(respaldo, 1.0);
    if (verColoresPrueba == 1) pixel = vec4(respaldo, 1.0);
    if (verColoresPrueba == 0 && pixel.a < 0.15) discard;
    vec3 normal = normalize(normalMundo);
    float luzSol = max(dot(normal, normalize(vec3(0.45, 1.0, 0.5))), 0.0);
    vec3 dirPunto = posicionLuz - posMundo;
    float dist = length(dirPunto);
    float luzPunto = max(dot(normal, normalize(dirPunto)), 0.0) / (1.0 + 0.16 * dist * dist);
    vec3 iluminado = pixel.rgb * (0.74 + 0.26 * luzSol + 0.43 * luzPunto);
    float alfa = esHoja == 1 ? pixel.a * opacidad : 1.0;
    FragColor = vec4(min(iluminado, vec3(1.0)), alfa);
}
)GLSL";

	GLuint vs = CompilarShaderArbol(GL_VERTEX_SHADER, vertex);
	GLuint fs = CompilarShaderArbol(GL_FRAGMENT_SHADER, fragment);
	if (!vs || !fs) {
		if (vs) glDeleteShader(vs);
		if (fs) glDeleteShader(fs);
		return 0;
	}
	GLuint programa = glCreateProgram();
	glAttachShader(programa, vs);
	glAttachShader(programa, fs);
	glLinkProgram(programa);
	glDeleteShader(vs);
	glDeleteShader(fs);
	GLint correcto = GL_FALSE;
	glGetProgramiv(programa, GL_LINK_STATUS, &correcto);
	if (correcto != GL_TRUE) {
		GLint tam = 0;
		glGetProgramiv(programa, GL_INFO_LOG_LENGTH, &tam);
		std::vector<GLchar> mensaje(static_cast<size_t>(std::max(tam, 1)));
		glGetProgramInfoLog(programa, static_cast<GLsizei>(mensaje.size()), nullptr, mensaje.data());
		std::cerr << "ERROR al enlazar shader del arbol: " << mensaje.data() << std::endl;
		glDeleteProgram(programa);
		return 0;
	}
	std::cout << "Shader del arbol: OK" << std::endl;
	return programa;
}

static bool CargarArbol(const std::string& archivo, const std::string& imagen, ArbolMinecraft& arbol) {
	std::ifstream entrada(archivo);
	if (!entrada.is_open()) {
		std::cerr << "ERROR: no se pudo abrir " << archivo << std::endl;
		return false;
	}
	std::vector<glm::vec3> posiciones, normales;
	std::vector<glm::vec2> uvs;
	std::vector<VerticeArbol> tronco, hojas;
	std::string linea, material;
	try {
		while (std::getline(entrada, linea)) {
			std::istringstream datos(linea);
			std::string tipo;
			datos >> tipo;
			if (tipo == "v") {
				glm::vec3 p;
				datos >> p.x >> p.y >> p.z;
				posiciones.push_back(p);
			}
			else if (tipo == "vt") {
				glm::vec2 uv;
				datos >> uv.x >> uv.y;
				uvs.push_back(uv);
			}
			else if (tipo == "vn") {
				glm::vec3 normal;
				datos >> normal.x >> normal.y >> normal.z;
				normales.push_back(normal);
			}
			else if (tipo == "usemtl") {
				datos >> material;
			}
			else if (tipo == "f") {
				std::vector<std::string> cara;
				std::string elemento;
				while (datos >> elemento) cara.push_back(elemento);
				if (cara.size() < 3) continue;
				std::vector<VerticeArbol>& destino = material == "Oak_Leaves" ? hojas : tronco;
				for (size_t i = 1; i + 1 < cara.size(); ++i) {
					AgregarVertice(cara[0], posiciones, uvs, normales, destino);
					AgregarVertice(cara[i], posiciones, uvs, normales, destino);
					AgregarVertice(cara[i + 1], posiciones, uvs, normales, destino);
				}
			}
		}
	}
	catch (const std::exception& error) {
		std::cerr << "ERROR leyendo OBJ: " << error.what() << std::endl;
		return false;
	}
	if (tronco.empty() || hojas.empty()) {
		std::cerr << "ERROR: el OBJ no tiene ambas partes del arbol." << std::endl;
		return false;
	}
	SubirMalla(tronco, arbol.tronco);
	SubirMalla(hojas, arbol.hojas);

	int ancho = 0, alto = 0, canales = 0;
	unsigned char* pixeles = SOIL_load_image(imagen.c_str(), &ancho, &alto, &canales, SOIL_LOAD_RGBA);
	if (pixeles && ancho > 0 && alto > 0) {
		// El OBJ usa UV desde abajo; el PNG se lee desde arriba.
		const size_t filaBytes = static_cast<size_t>(ancho) * 4;
		std::vector<unsigned char> imagenInvertida(filaBytes * static_cast<size_t>(alto));
		for (int fila = 0; fila < alto; ++fila) {
			std::copy(pixeles + static_cast<size_t>(fila) * filaBytes,
				pixeles + static_cast<size_t>(fila + 1) * filaBytes,
				imagenInvertida.data() + static_cast<size_t>(alto - 1 - fila) * filaBytes);
		}
		SOIL_free_image_data(pixeles);
		glGenTextures(1, &arbol.textura);
		glBindTexture(GL_TEXTURE_2D, arbol.textura);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagenInvertida.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		std::cout << "Textura cargada: " << ancho << "x" << alto << " RGBA (UV corregidas)" << std::endl;
	}
	else {
		if (pixeles) SOIL_free_image_data(pixeles);
		std::cerr << "ADVERTENCIA: no se leyo el atlas: " << SOIL_last_result()
			<< ". Mostrare tronco cafe y hojas verdes para no ocultar el arbol." << std::endl;
	}

	std::cout << "Arbol cargado: " << tronco.size() / 3 << " triangulos de tronco y "
		<< hojas.size() / 3 << " triangulos de hojas." << std::endl;
	return true;
}

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Previo 9 Bello Zaragoza Demian", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	std::cout << "=== VERSION ARBOL RGB+ALFA / CONTROL E-Q / OCT-2026 ===" << std::endl;

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);

	// El cursor queda capturado para mirar en cualquier direccion. TAB lo libera.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	firstMouse = true;

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	GLuint programaArbol = CrearProgramaArbol();
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;


	char workingDir[1024] = { 0 };
	if (_getcwd(workingDir, sizeof(workingDir)))
		std::cout << "Directorio de ejecucion: " << workingDir << std::endl;
	const char* filesToCheck[] = {
		"Models/MinecraftTree/Mineways2Skfb.obj",
		"Models/MinecraftTree/Mineways2Skfb.mtl",
		"Models/MinecraftTree/Mineways2Skfb-RGBA.png"
	};
	bool completeTree = true;
	for (int i = 0; i < 3; ++i) {
		std::ifstream file(filesToCheck[i], std::ios::binary);
		std::cout << (file.good() ? "ENCONTRADO: " : "NO ENCONTRADO: ") << filesToCheck[i] << std::endl;
		if (!file.good()) completeTree = false;
	}
	if (!completeTree)
		std::cout << "ERROR: copia la carpeta MinecraftTree dentro de Models, en el directorio de ejecucion indicado arriba." << std::endl;
	ArbolMinecraft arbol;
	if (!CargarArbol("Models/MinecraftTree/Mineways2Skfb.obj",
		"Models/MinecraftTree/Mineways2Skfb-RGBA.png", arbol)) {
		std::cerr << "No se pudo iniciar el arbol." << std::endl;
	}
	Model Piso((char*)"Models/piso.obj");



	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);



		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y + alturaCamara, camera.GetPosition().z);


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y + alturaCamara, camera.GetPosition().z);
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
		view = camera.GetViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -alturaCamara, 0.0f));

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);



		//Carga de modelo 
		view = camera.GetViewMatrix() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -alturaCamara, 0.0f));
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);



		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.15f, -1.45f, -2.1f));
		model = glm::scale(model, glm::vec3(2.4f));

		// El tronco se dibuja primero, completamente opaco.
		if (programaArbol && arbol.tronco.vao && arbol.hojas.vao) {
			glUseProgram(programaArbol);
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform3fv(glGetUniformLocation(programaArbol, "posicionLuz"), 1, glm::value_ptr(pointLightPositions[0]));
			glUniform1i(glGetUniformLocation(programaArbol, "atlas"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), arbol.textura ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "verColoresPrueba"), coloresDePrueba ? 1 : 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, arbol.textura);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 0);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), 1.0f);
			glBindVertexArray(arbol.tronco.vao);
			glDrawArrays(GL_TRIANGLES, 0, arbol.tronco.cantidad);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Las hojas se dibujan al final, una vez dibujada la luz.
		// Primero guardamos la profundidad de la hoja mas cercana.
		// Asi las capas interiores no suman opacidad unas con otras.
		if (programaArbol && arbol.hojas.vao) {
			glUseProgram(programaArbol);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 1);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), hojasTransparentes ? 0.30f : 1.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, arbol.textura);
			glBindVertexArray(arbol.hojas.vao);

			if (hojasTransparentes) {
				// Primera pasada: solo profundidad, sin alterar el color.
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
				glDepthFunc(GL_LESS);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glDrawArrays(GL_TRIANGLES, 0, arbol.hojas.cantidad);

				// Segunda pasada: mezclar una unica superficie al 30 %.
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glDepthFunc(GL_EQUAL);
				glDepthMask(GL_FALSE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDrawArrays(GL_TRIANGLES, 0, arbol.hojas.cantidad);
			}
			else {
				glDisable(GL_BLEND);
				glDepthMask(GL_TRUE);
				glDepthFunc(GL_LESS);
				glDrawArrays(GL_TRIANGLES, 0, arbol.hojas.cantidad);
			}

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	if (programaArbol) glDeleteProgram(programaArbol);
	if (arbol.textura) glDeleteTextures(1, &arbol.textura);
	if (arbol.tronco.vao) glDeleteVertexArrays(1, &arbol.tronco.vao);
	if (arbol.tronco.vbo) glDeleteBuffers(1, &arbol.tronco.vbo);
	if (arbol.hojas.vao) glDeleteVertexArrays(1, &arbol.hojas.vao);
	if (arbol.hojas.vbo) glDeleteBuffers(1, &arbol.hojas.vbo);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	float velocidadVertical = (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) ? 6.0f : 2.5f;
	if (keys[GLFW_KEY_E]) alturaCamara += velocidadVertical * deltaTime;
	if (keys[GLFW_KEY_Q]) alturaCamara -= velocidadVertical * deltaTime;

	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.01f;
	}

	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.01f;
	}

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		mouseCaptured = !mouseCaptured;
		glfwSetInputMode(window, GLFW_CURSOR,
			mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		firstMouse = true;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		hojasTransparentes = !hojasTransparentes;
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		coloresDePrueba = !coloresDePrueba;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
	// Evita un salto brusco al regresar a la ventana con Alt+Tab.
	firstMouse = true;
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (!mouseCaptured) return;

	if (firstMouse)
	{
		lastX = static_cast<GLfloat>(xPos);
		lastY = static_cast<GLfloat>(yPos);
		firstMouse = false;
		return;
	}

	GLfloat xOffset = static_cast<GLfloat>(xPos - lastX);
	GLfloat yOffset = static_cast<GLfloat>(lastY - yPos);

	lastX = static_cast<GLfloat>(xPos);
	lastY = static_cast<GLfloat>(yPos);

	camera.ProcessMouseMovement(xOffset, yOffset);
}
