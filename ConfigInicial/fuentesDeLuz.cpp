// Practica #9 
// Bello Zaragoza Demian
// Fecha de entrega: 17 de octubre 2026
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
#include <limits>

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
bool active = true;
float alturaCamara = 0.0f;
bool coloresDePrueba = false;

// Cinco fuentes de escena: luna direccional, portal tipo spotlight y tres
// luces puntuales (antorcha, glowstone y lava). Hay tres tipos;
// el modelo conserva una contribucion ambiental tenue como relleno.
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
	float baseLocal = 0.0f;
};

// El piso usa Models/MinecraftTree/grass.png completo.
const float PASTO_U_MIN = 0.0f;
const float PASTO_U_MAX = 1.0f;
const float PASTO_V_MIN = 0.0f;
const float PASTO_V_MAX = 1.0f;
// Unidad base: cada bloque cubico del escenario mide exactamente 1 x 1 x 1.
const float UNIDAD_BLOQUE_MINECRAFT = 1.0f;
const int BALDOSAS_POR_LADO = 40;
const float LADO_BALDOSA = UNIDAD_BLOQUE_MINECRAFT;
const int BLOQUES_LAVA_X = 2;
const int BLOQUES_LAVA_Z = 3;
const float LADO_BLOQUE_LAVA = UNIDAD_BLOQUE_MINECRAFT;
const float DISTANCIA_LAGO_LAVA = 2.7f; // Desplazamiento hacia el frente (+Z), medido desde el perro.
const glm::vec3 POSICION_ARBOL(-2.60f, -1.45f, -4.30f); // A la izquierda y mas al fondo que el perro.
const float ESCALA_ARBOL_ANTERIOR = 2.4f; // Solo conserva la altura actual del piso.
const float ESCALA_ARBOL = 5.50f;         // Aproximadamente seis bloques de alto; el tronco queda apoyado en el mismo piso.

// AJUSTES MANUALES DE LA ANTORCHA.
// Modifica estos tres valores si deseas moverla, girarla o cambiar su tamano.
const glm::vec3 POSICION_ANTORCHA(1.35f, 0.0f, -1.20f);
const glm::vec3 ROTACION_ANTORCHA(0.0f, 0.0f, 0.0f);
const float ESCALA_ANTORCHA = 0.58f; // Una antorcha de Minecraft mide menos de un bloque.

// SEGUNDA ANTORCHA, colocada a la izquierda del perrito.
const glm::vec3 POSICION_ANTORCHA_2(-2.35f, 0.0f, -0.70f);
const glm::vec3 ROTACION_ANTORCHA_2(0.0f, 0.0f, 0.0f);
const float ESCALA_ANTORCHA_2 = 0.58f;

// AJUSTES MANUALES DEL PERRITO.
const glm::vec3 POSICION_PERRO(-1.25f, 0.0f, -0.80f);
const glm::vec3 ROTACION_PERRO(0.0f, 25.0f, 0.0f);
const float ESCALA_PERRO = 1.0f;

// AJUSTES MANUALES DEL PORTAL.
const glm::vec3 POSICION_PORTAL(3.20f, 0.0f, -4.20f);
const glm::vec3 ROTACION_PORTAL(0.0f, 0.0f, 0.0f);
const float ESCALA_PORTAL = 1.0f;

// AJUSTES MANUALES DEL BLOQUE DE GLOWSTONE.
const glm::vec3 POSICION_GLOWSTONE(3.50f, 0.0f, -0.45f); // Desplazada a la derecha.
const glm::vec3 ROTACION_GLOWSTONE(0.0f, 0.0f, 0.0f);
const float ESCALA_GLOWSTONE = UNIDAD_BLOQUE_MINECRAFT; // Mismo tamano que cada bloque del piso y de lava.

// LUNA 3D: bloque texturizado colocado arriba y detras de la escena.
const glm::vec3 POSICION_LUNA(17.50f, 18.00f, -33.00f); // Un poco mas a la derecha (+X) y al frente (+Z).
const glm::vec3 ROTACION_LUNA(0.0f, 0.0f, 0.0f);
const float ESCALA_LUNA = 3.80f;

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

static void CrearPisoMinecraft(ParteArbol& piso, float altura) {
	std::vector<VerticeArbol> verticesPiso;
	verticesPiso.reserve(BALDOSAS_POR_LADO * BALDOSAS_POR_LADO * 6);
	const float inicioX = POSICION_ARBOL.x - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
	const float inicioZ = POSICION_ARBOL.z - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
	// Reserva un parche compacto de 2 x 3 baldosas delante del perro (+Z).
	// La base se calcula en indices de cuadricula para que no queden huecos.
	const int celdaPerroX = static_cast<int>(std::floor((POSICION_PERRO.x - inicioX) / LADO_BALDOSA));
	const int celdaObjetivoZ = static_cast<int>(std::floor((POSICION_PERRO.z + DISTANCIA_LAGO_LAVA - inicioZ) / LADO_BALDOSA));
	const int primeraCeldaX = celdaPerroX - BLOQUES_LAVA_X / 2;
	const int primeraCeldaZ = celdaObjetivoZ - BLOQUES_LAVA_Z / 2;
	for (int z = 0; z < BALDOSAS_POR_LADO; ++z) {
		for (int x = 0; x < BALDOSAS_POR_LADO; ++x) {
			float x0 = inicioX + x * LADO_BALDOSA;
			float x1 = x0 + LADO_BALDOSA;
			float z0 = inicioZ + z * LADO_BALDOSA;
			float z1 = z0 + LADO_BALDOSA;
			if (x >= primeraCeldaX && x < primeraCeldaX + BLOQUES_LAVA_X &&
				z >= primeraCeldaZ && z < primeraCeldaZ + BLOQUES_LAVA_Z)
				continue;
			glm::vec3 normal(0.0f, 1.0f, 0.0f);
			VerticeArbol a = { glm::vec3(x0, altura, z0), normal, glm::vec2(PASTO_U_MIN, PASTO_V_MIN) };
			VerticeArbol b = { glm::vec3(x0, altura, z1), normal, glm::vec2(PASTO_U_MIN, PASTO_V_MAX) };
			VerticeArbol c = { glm::vec3(x1, altura, z0), normal, glm::vec2(PASTO_U_MAX, PASTO_V_MIN) };
			VerticeArbol d = { glm::vec3(x1, altura, z1), normal, glm::vec2(PASTO_U_MAX, PASTO_V_MAX) };
			verticesPiso.push_back(a); verticesPiso.push_back(b); verticesPiso.push_back(c);
			verticesPiso.push_back(c); verticesPiso.push_back(b); verticesPiso.push_back(d);
		}
	}
	SubirMalla(verticesPiso, piso);
}

static void CrearCuboTexturizado(ParteArbol& cubo) {
	std::vector<VerticeArbol> v;
	v.reserve(36);
	auto cara = [&v](glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 n) {
		v.push_back({ a, n, glm::vec2(0, 0) });
		v.push_back({ b, n, glm::vec2(1, 0) });
		v.push_back({ c, n, glm::vec2(1, 1) });
		v.push_back({ a, n, glm::vec2(0, 0) });
		v.push_back({ c, n, glm::vec2(1, 1) });
		v.push_back({ d, n, glm::vec2(0, 1) });
		};
	const float m = 0.5f;
	cara({ -m,-m, m }, { m,-m, m }, { m, m, m }, { -m, m, m }, { 0, 0, 1 });
	cara({ m,-m,-m }, { -m,-m,-m }, { -m, m,-m }, { m, m,-m }, { 0, 0,-1 });
	cara({ -m,-m,-m }, { -m,-m, m }, { -m, m, m }, { -m, m,-m }, { -1, 0, 0 });
	cara({ m,-m, m }, { m,-m,-m }, { m, m,-m }, { m, m, m }, { 1, 0, 0 });
	cara({ -m, m, m }, { m, m, m }, { m, m,-m }, { -m, m,-m }, { 0, 1, 0 });
	cara({ -m,-m,-m }, { m,-m,-m }, { m,-m, m }, { -m,-m, m }, { 0,-1, 0 });
	SubirMalla(v, cubo);
}

static GLuint CargarTexturaRGBA(const std::string& ruta) {
	int ancho = 0, alto = 0, canales = 0;
	unsigned char* pixeles = SOIL_load_image(ruta.c_str(), &ancho, &alto, &canales, SOIL_LOAD_RGBA);
	if (!pixeles || ancho <= 0 || alto <= 0) {
		if (pixeles) SOIL_free_image_data(pixeles);
		std::cerr << "ERROR: no se pudo cargar " << ruta << ": " << SOIL_last_result() << std::endl;
		return 0;
	}

	// OpenGL espera que la primera fila de la textura sea la inferior.
	const size_t bytesFila = static_cast<size_t>(ancho) * 4;
	std::vector<unsigned char> invertida(bytesFila * static_cast<size_t>(alto));
	for (int fila = 0; fila < alto; ++fila) {
		std::copy(pixeles + static_cast<size_t>(fila) * bytesFila,
			pixeles + static_cast<size_t>(fila + 1) * bytesFila,
			invertida.data() + static_cast<size_t>(alto - 1 - fila) * bytesFila);
	}
	SOIL_free_image_data(pixeles);

	GLuint textura = 0;
	glGenTextures(1, &textura);
	glBindTexture(GL_TEXTURE_2D, textura);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ancho, alto, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, invertida.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "Textura cargada: " << ruta << " (" << ancho << "x" << alto << ")" << std::endl;
	return textura;
}

static bool CargarOBJSimple(const std::string& ruta, ParteArbol& malla) {
	std::ifstream entrada(ruta);
	if (!entrada.is_open()) {
		std::cerr << "ERROR: no se pudo abrir " << ruta << std::endl;
		return false;
	}
	std::vector<glm::vec3> posiciones, normales;
	std::vector<glm::vec2> uvs;
	std::vector<VerticeArbol> verticesModelo;
	std::string linea;
	try {
		while (std::getline(entrada, linea)) {
			std::istringstream datos(linea);
			std::string tipo;
			datos >> tipo;
			if (tipo == "v") {
				glm::vec3 p; datos >> p.x >> p.y >> p.z; posiciones.push_back(p);
			}
			else if (tipo == "vt") {
				glm::vec2 uv; datos >> uv.x >> uv.y; uvs.push_back(uv);
			}
			else if (tipo == "vn") {
				glm::vec3 n; datos >> n.x >> n.y >> n.z; normales.push_back(n);
			}
			else if (tipo == "f") {
				std::vector<std::string> cara;
				std::string elemento;
				while (datos >> elemento) cara.push_back(elemento);
				for (size_t i = 1; i + 1 < cara.size(); ++i) {
					AgregarVertice(cara[0], posiciones, uvs, normales, verticesModelo);
					AgregarVertice(cara[i], posiciones, uvs, normales, verticesModelo);
					AgregarVertice(cara[i + 1], posiciones, uvs, normales, verticesModelo);
				}
			}
		}
	}
	catch (const std::exception& error) {
		std::cerr << "ERROR leyendo " << ruta << ": " << error.what() << std::endl;
		return false;
	}
	if (verticesModelo.empty()) return false;
	SubirMalla(verticesModelo, malla);
	std::cout << "OBJ cargado manualmente: " << ruta << std::endl;
	return true;
}

static bool MedirOBJ(const std::string& ruta, glm::vec3& minimo, glm::vec3& maximo) {
	std::ifstream archivo(ruta);
	if (!archivo.is_open()) return false;
	float limite = (std::numeric_limits<float>::max)();
	minimo = glm::vec3(limite);
	maximo = glm::vec3(-limite);
	bool encontrado = false;
	std::string linea;
	while (std::getline(archivo, linea)) {
		std::istringstream datos(linea);
		std::string tipo;
		glm::vec3 p;
		datos >> tipo;
		if (tipo == "v" && (datos >> p.x >> p.y >> p.z)) {
			minimo = glm::min(minimo, p);
			maximo = glm::max(maximo, p);
			encontrado = true;
		}
	}
	return encontrado;
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
uniform vec3 posicionLuz2;
uniform vec3 posicionLuz3;
uniform vec3 posicionSpotPortal;
uniform vec3 direccionSpotPortal;
uniform int esHoja;
uniform int texturaCorrecta;
uniform float opacidad;
uniform int verColoresPrueba;
uniform int esAntorcha;
uniform int esPortal;
uniform int esLuna;
uniform int esLava;
uniform int esGlowstone;
out vec4 FragColor;
void main() {
    vec3 respaldo = esHoja == 1 ? vec3(0.24, 0.68, 0.25) : vec3(0.53, 0.32, 0.16);
    vec4 pixel = texturaCorrecta == 1 ? texture(atlas, uv) : vec4(respaldo, 1.0);
    if (verColoresPrueba == 1) pixel = vec4(respaldo, 1.0);
    if (verColoresPrueba == 0 && pixel.a < 0.15) discard;
    vec3 normal = normalize(normalMundo);
    // La luna del cielo ilumina desde arriba y desde la derecha.
    float luzLuna = max(dot(normal, normalize(vec3(-0.45, 1.0, 0.35))), 0.0);
    vec3 dirPunto = posicionLuz - posMundo;
    float dist = length(dirPunto);
    float luzPunto = max(dot(normal, normalize(dirPunto)), 0.0) / (1.0 + 0.16 * dist * dist);
    vec3 dirPunto2 = posicionLuz2 - posMundo;
    float dist2 = length(dirPunto2);
    float luzPunto2 = max(dot(normal, normalize(dirPunto2)), 0.0) / (1.0 + 0.07 * dist2 * dist2);
    vec3 dirPunto3 = posicionLuz3 - posMundo;
    float dist3 = length(dirPunto3);
    // La lava tiene mayor alcance y potencia para que ilumine visiblemente
    // el piso, al perro y los objetos cercanos, no solo la cara superior del charco.
    float luzPunto3 = max(dot(normal, normalize(dirPunto3)), 0.0) /
        (1.0 + 0.035 * dist3 + 0.008 * dist3 * dist3);
    float haloLava = 0.70 / (1.0 + 0.055 * dist3 * dist3);
    // Spotlight morado del portal: cono dirigido hacia el perro y el terreno.
    vec3 dirSpotPortal = posicionSpotPortal - posMundo;
    float distSpotPortal = length(dirSpotPortal);
    float thetaPortal = dot(normalize(dirSpotPortal), normalize(-direccionSpotPortal));
    float conoPortal = smoothstep(cos(radians(30.0)), cos(radians(18.0)), thetaPortal);
    float luzSpotPortal = max(dot(normal, normalize(dirSpotPortal)), 0.0) * conoPortal /
        (1.0 + 0.08 * distSpotPortal + 0.018 * distSpotPortal * distSpotPortal);
    vec3 ambienteNocturno = pixel.rgb * vec3(0.12, 0.15, 0.24);
    vec3 iluminacionLuna = pixel.rgb * vec3(0.28, 0.34, 0.52) * luzLuna;
    vec3 iluminacionAntorcha = pixel.rgb * vec3(1.0, 0.58, 0.22) * (0.72 * luzPunto);
    vec3 iluminacionGlowstone = pixel.rgb * vec3(1.0, 0.78, 0.34) * (1.35 * luzPunto2);
    vec3 iluminacionLava = pixel.rgb * vec3(1.0, 0.30, 0.035) * (2.00 * luzPunto3 + haloLava);
    vec3 iluminacionPortal = pixel.rgb * vec3(0.48, 0.16, 1.0) * (1.35 * luzSpotPortal);
    vec3 iluminado = ambienteNocturno + iluminacionLuna + iluminacionAntorcha + iluminacionGlowstone + iluminacionLava + iluminacionPortal;
    if (esAntorcha == 1)
        iluminado = max(iluminado, pixel.rgb * vec3(0.95, 0.82, 0.62));
    if (esGlowstone == 1)
        iluminado = max(iluminado, pixel.rgb * vec3(1.50, 1.30, 0.72));
    if (esPortal == 1)
        iluminado = max(iluminado, pixel.rgb * vec3(0.92, 0.58, 1.0));
    if (esLuna == 1)
        iluminado = max(iluminado, pixel.rgb * vec3(1.10, 1.14, 1.25));
    if (esLava == 1)
        iluminado = max(iluminado, pixel.rgb * vec3(1.65, 0.62, 0.08));
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

static GLuint CrearProgramaCielo() {
	const char* vertex = R"GLSL(#version 330 core
out vec2 uv;
void main() {
    vec2 posiciones[3] = vec2[3](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    vec2 coordenadas[3] = vec2[3](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );
    uv = coordenadas[gl_VertexID];
    gl_Position = vec4(posiciones[gl_VertexID], 0.9999, 1.0);
}
)GLSL";
	const char* fragment = R"GLSL(#version 330 core
in vec2 uv;
out vec4 FragColor;
uniform vec2 resolucion;

void main() {
    float alto = clamp(uv.y, 0.0, 1.0);
    vec3 horizonte = vec3(0.11, 0.17, 0.38);
    vec3 cenit = vec3(0.006, 0.010, 0.045);
    vec3 cielo = mix(horizonte, cenit, smoothstep(0.0, 0.88, alto));

    FragColor = vec4(cielo, 1.0);
}
)GLSL";
	GLuint vs = CompilarShaderArbol(GL_VERTEX_SHADER, vertex);
	GLuint fs = CompilarShaderArbol(GL_FRAGMENT_SHADER, fragment);
	if (!vs || !fs) return 0;
	GLuint programa = glCreateProgram();
	glAttachShader(programa, vs); glAttachShader(programa, fs);
	glLinkProgram(programa);
	glDeleteShader(vs); glDeleteShader(fs);
	GLint correcto = GL_FALSE;
	glGetProgramiv(programa, GL_LINK_STATUS, &correcto);
	if (correcto != GL_TRUE) {
		GLint tam = 0;
		glGetProgramiv(programa, GL_INFO_LOG_LENGTH, &tam);
		std::vector<GLchar> mensaje(static_cast<size_t>(std::max(tam, 1)));
		glGetProgramInfoLog(programa, static_cast<GLsizei>(mensaje.size()), nullptr, mensaje.data());
		std::cerr << "ERROR al enlazar shader del cielo: " << mensaje.data() << std::endl;
		glDeleteProgram(programa);
		return 0;
	}
	std::cout << "Shader del cielo nocturno: OK" << std::endl;
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
	if (!posiciones.empty()) {
		arbol.baseLocal = posiciones[0].y;
		for (const glm::vec3& p : posiciones)
			arbol.baseLocal = std::min(arbol.baseLocal, p.y);
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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 9 Bello Zaragoza Demian", nullptr, nullptr);

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
	GLuint programaCielo = CrearProgramaCielo();
	GLuint vaoCielo = 0;
	glGenVertexArrays(1, &vaoCielo);
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;


	char workingDir[1024] = { 0 };
	if (_getcwd(workingDir, sizeof(workingDir)))
		std::cout << "Directorio de ejecucion: " << workingDir << std::endl;
	const char* filesToCheck[] = {
		"Models/MinecraftTree/Mineways2Skfb.obj",
		"Models/MinecraftTree/Mineways2Skfb.mtl",
		"Models/MinecraftTree/Mineways2Skfb-RGBA.png",
		"Models/MinecraftTree/grass.png"
	};
	bool completeTree = true;
	for (int i = 0; i < 4; ++i) {
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
	ParteArbol pisoMinecraft;
	// Se conserva la altura que tenia el piso antes de agrandar el arbol.
	const float alturaPiso = POSICION_ARBOL.y + ESCALA_ARBOL_ANTERIOR * arbol.baseLocal - 0.005f;
	CrearPisoMinecraft(pisoMinecraft, alturaPiso);
	GLuint texturaPasto = CargarTexturaRGBA("Models/MinecraftTree/grass.png");
	// La antorcha se carga manualmente para ignorar la ruta rota de Torch.mtl.
	ParteArbol antorcha;
	CargarOBJSimple("Models/minecraft-torch/Torch.obj", antorcha);
	GLuint texturaAntorcha = CargarTexturaRGBA("Models/minecraft-torch/Diffuse.png");
	glm::vec3 antorchaMin(0.0f), antorchaMax(1.0f);
	float escalaNormalAntorcha = 1.0f;
	if (MedirOBJ("Models/minecraft-torch/Torch.obj", antorchaMin, antorchaMax)) {
		float alturaOriginal = antorchaMax.y - antorchaMin.y;
		if (alturaOriginal > 0.0001f)
			escalaNormalAntorcha = 1.10f / alturaOriginal;
	}
	pointLightPositions[0] = glm::vec3(POSICION_ANTORCHA.x,
		alturaPiso + POSICION_ANTORCHA.y + 0.95f * ESCALA_ANTORCHA,
		POSICION_ANTORCHA.z);
	std::cout << "Antorcha cargada: Models/minecraft-torch/Torch.obj" << std::endl;

	// Portal del Nether: OBJ y textura se cargan por separado para evitar
	// problemas con rutas internas del archivo MTL.
	ParteArbol portal;
	CargarOBJSimple("Models/minecraft-portal/NetherPortal.obj", portal);
	GLuint texturaPortal = CargarTexturaRGBA("Models/minecraft-portal/NetherPortal.png");
	glm::vec3 portalMin(0.0f), portalMax(1.0f);
	float escalaNormalPortal = 1.0f;
	if (MedirOBJ("Models/minecraft-portal/NetherPortal.obj", portalMin, portalMax)) {
		float alturaOriginal = portalMax.y - portalMin.y;
		if (alturaOriginal > 0.0001f)
			escalaNormalPortal = 5.0f / alturaOriginal;
	}

	// Bloque de glowstone creado en el programa; cada cara usa la textura completa.
	ParteArbol glowstone;
	CrearCuboTexturizado(glowstone);
	GLuint texturaGlowstone = CargarTexturaRGBA("Models/MinecraftTree/glowstone.png");
	GLuint texturaLava = CargarTexturaRGBA("Models/MinecraftTree/lava.png");
	ParteArbol luna;
	CrearCuboTexturizado(luna);
	GLuint texturaLuna = CargarTexturaRGBA("Models/MinecraftTree/moon.png");
	pointLightPositions[1] = glm::vec3(POSICION_GLOWSTONE.x,
		alturaPiso + POSICION_GLOWSTONE.y + 0.5f * ESCALA_GLOWSTONE,
		POSICION_GLOWSTONE.z);
	// Tercera luz puntual: resplandor naranja emitido por el centro del charco de lava.
	const float inicioPisoLuzX = POSICION_ARBOL.x - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
	const float inicioPisoLuzZ = POSICION_ARBOL.z - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
	const int celdaLavaX = static_cast<int>(std::floor((POSICION_PERRO.x - inicioPisoLuzX) / LADO_BALDOSA));
	const int celdaLavaZ = static_cast<int>(std::floor((POSICION_PERRO.z + DISTANCIA_LAGO_LAVA - inicioPisoLuzZ) / LADO_BALDOSA));
	const int primeraLavaX = celdaLavaX - BLOQUES_LAVA_X / 2;
	const int primeraLavaZ = celdaLavaZ - BLOQUES_LAVA_Z / 2;
	pointLightPositions[2] = glm::vec3(
		inicioPisoLuzX + (primeraLavaX + BLOQUES_LAVA_X * 0.5f) * LADO_BALDOSA,
		alturaPiso + 0.35f, // La fuente queda ligeramente encima del charco para irradiar alrededor.
		inicioPisoLuzZ + (primeraLavaZ + BLOQUES_LAVA_Z * 0.5f) * LADO_BALDOSA);
	// Quinta fuente: spotlight violeta que nace en el portal y apunta hacia el perro.
	const glm::vec3 posicionSpotPortal(POSICION_PORTAL.x,
		alturaPiso + POSICION_PORTAL.y + 2.7f, POSICION_PORTAL.z + 0.45f);
	const glm::vec3 objetivoSpotPortal(POSICION_PERRO.x,
		alturaPiso + POSICION_PERRO.y + 0.65f, POSICION_PERRO.z);
	const glm::vec3 direccionSpotPortal = glm::normalize(objetivoSpotPortal - posicionSpotPortal);

	Model dog((char*)"Models/RedDog.obj");
	glm::vec3 perroMin(0.0f), perroMax(1.0f);
	float escalaNormalPerro = 1.0f;
	if (MedirOBJ("Models/RedDog.obj", perroMin, perroMax)) {
		float alturaOriginal = perroMax.y - perroMin.y;
		if (alturaOriginal > 0.0001f)
			escalaNormalPerro = 1.20f / alturaOriginal;
	}



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
		// Color nocturno de respaldo, visible incluso si el shader del cielo falla.
		glClearColor(0.018f, 0.028f, 0.090f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Fondo nocturno limpio: solo degradado azul, sin luna procedural, nubes ni estrellas.
		if (programaCielo) {
			glDisable(GL_DEPTH_TEST);
			glUseProgram(programaCielo);
			glUniform2f(glGetUniformLocation(programaCielo, "resolucion"),
				static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT));
			glBindVertexArray(vaoCielo);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		}

		// OpenGL options
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);



		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y + alturaCamara, camera.GetPosition().z);


		// Fuente 1/5: luz direccional azulada de la luna.
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.45f, -1.0f, -0.35f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.12f, 0.14f, 0.22f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.38f, 0.44f, 0.62f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.45f, 0.52f, 0.75f);


		// Fuente 2/5: punto calido de la primera antorcha.
		glm::vec3 lightColor = active
			? glm::vec3(1.0f, 0.48f, 0.12f)
			: glm::vec3(0.0f);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);



		// Fuente 3/5: punto dorado de la glowstone.
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.34f, 0.22f, 0.07f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 0.88f, 0.46f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 0.75f, 0.28f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.014f);

		// Fuente 4/5: luz puntual naranja de la lava.
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.42f, 0.105f, 0.012f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 1.0f, 0.34f, 0.035f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 0.42f, 0.06f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.025f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.008f);

		// Point light 4: ranura reservada, sin contribucion.
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);

		// Fuente 5/5: spotlight violeta del portal (tercer tipo de iluminacion).
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), posicionSpotPortal.x, posicionSpotPortal.y, posicionSpotPortal.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), direccionSpotPortal.x, direccionSpotPortal.y, direccionSpotPortal.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.035f, 0.015f, 0.08f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.55f, 0.18f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.08f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.018f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(18.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(30.0f)));

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

		// Perrito 3D con tamano normalizado y apoyado sobre el terreno.
		lightingShader.Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(
			POSICION_PERRO.x, alturaPiso + POSICION_PERRO.y, POSICION_PERRO.z));
		model = glm::rotate(model, glm::radians(ROTACION_PERRO.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ROTACION_PERRO.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ROTACION_PERRO.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaNormalPerro * ESCALA_PERRO));
		model = glm::translate(model, glm::vec3(
			-(perroMin.x + perroMax.x) * 0.5f,
			-perroMin.y,
			-(perroMin.z + perroMax.z) * 0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		dog.Draw(lightingShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(
			POSICION_ARBOL.x,
			alturaPiso - ESCALA_ARBOL * arbol.baseLocal,
			POSICION_ARBOL.z));
		model = glm::scale(model, glm::vec3(ESCALA_ARBOL));

		// El tronco se dibuja primero, completamente opaco.
		if (programaArbol && arbol.tronco.vao && arbol.hojas.vao) {
			glUseProgram(programaArbol);
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniform3fv(glGetUniformLocation(programaArbol, "posicionLuz"), 1, glm::value_ptr(pointLightPositions[0]));
			glUniform3fv(glGetUniformLocation(programaArbol, "posicionLuz2"), 1, glm::value_ptr(pointLightPositions[1]));
			glUniform3fv(glGetUniformLocation(programaArbol, "posicionLuz3"), 1, glm::value_ptr(pointLightPositions[2]));
			glUniform3fv(glGetUniformLocation(programaArbol, "posicionSpotPortal"), 1, glm::value_ptr(posicionSpotPortal));
			glUniform3fv(glGetUniformLocation(programaArbol, "direccionSpotPortal"), 1, glm::value_ptr(direccionSpotPortal));
			glUniform1i(glGetUniformLocation(programaArbol, "atlas"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), arbol.textura ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "verColoresPrueba"), coloresDePrueba ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLuna"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLava"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esGlowstone"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texturaPasto);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			// Cada cuadro del piso repite la imagen grass.png completa.
			// esHoja=1 da respaldo verde si falta el PNG; opacidad=1 mantiene el piso opaco.
			glm::mat4 modeloPiso(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloPiso));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaPasto ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 1);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), 1.0f);
			glBindVertexArray(pisoMinecraft.vao);
			glDrawArrays(GL_TRIANGLES, 0, pisoMinecraft.cantidad);
			glBindVertexArray(0);

			// Seis bloques forman un charco compacto de 2 x 3 delante del perro.
			// Sus caras superiores quedan exactamente al nivel del piso; el cuerpo
			// del cubo baja bajo el terreno para que no sobresalga como un bloque.
			const float inicioPisoX = POSICION_ARBOL.x - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
			const float inicioPisoZ = POSICION_ARBOL.z - BALDOSAS_POR_LADO * LADO_BALDOSA * 0.5f;
			const int celdaPerroX = static_cast<int>(std::floor((POSICION_PERRO.x - inicioPisoX) / LADO_BALDOSA));
			const int celdaObjetivoZ = static_cast<int>(std::floor((POSICION_PERRO.z + DISTANCIA_LAGO_LAVA - inicioPisoZ) / LADO_BALDOSA));
			const int primeraCeldaX = celdaPerroX - BLOQUES_LAVA_X / 2;
			const int primeraCeldaZ = celdaObjetivoZ - BLOQUES_LAVA_Z / 2;
			glBindTexture(GL_TEXTURE_2D, texturaLava);
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaLava ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLuna"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLava"), 1);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), 1.0f);
			glBindVertexArray(glowstone.vao);
			for (int z = 0; z < BLOQUES_LAVA_Z; ++z) {
				for (int x = 0; x < BLOQUES_LAVA_X; ++x) {
					const float centroBloqueX = inicioPisoX +
						(primeraCeldaX + x + 0.5f) * LADO_BALDOSA;
					const float centroBloqueZ = inicioPisoZ +
						(primeraCeldaZ + z + 0.5f) * LADO_BALDOSA;
					glm::mat4 modeloLava(1.0f);
					modeloLava = glm::translate(modeloLava, glm::vec3(
						centroBloqueX, alturaPiso - 0.5f * LADO_BLOQUE_LAVA, centroBloqueZ));
					modeloLava = glm::scale(modeloLava, glm::vec3(LADO_BLOQUE_LAVA));
					glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
						glm::value_ptr(modeloLava));
					glDrawArrays(GL_TRIANGLES, 0, glowstone.cantidad);
				}
			}
			glBindVertexArray(0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLava"), 0);
			glBindTexture(GL_TEXTURE_2D, arbol.textura);
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), arbol.textura ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 0);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), 1.0f);
			glBindVertexArray(arbol.tronco.vao);
			glDrawArrays(GL_TRIANGLES, 0, arbol.tronco.cantidad);
			glBindVertexArray(0);

			// Antorcha: se fuerza Diffuse.png y se ignora la ruta incorrecta del MTL.
			glm::mat4 modeloAntorcha(1.0f);
			modeloAntorcha = glm::translate(modeloAntorcha, glm::vec3(
				POSICION_ANTORCHA.x, alturaPiso + POSICION_ANTORCHA.y, POSICION_ANTORCHA.z));
			modeloAntorcha = glm::rotate(modeloAntorcha, glm::radians(ROTACION_ANTORCHA.x), glm::vec3(1, 0, 0));
			modeloAntorcha = glm::rotate(modeloAntorcha, glm::radians(ROTACION_ANTORCHA.y), glm::vec3(0, 1, 0));
			modeloAntorcha = glm::rotate(modeloAntorcha, glm::radians(ROTACION_ANTORCHA.z), glm::vec3(0, 0, 1));
			modeloAntorcha = glm::scale(modeloAntorcha, glm::vec3(escalaNormalAntorcha * ESCALA_ANTORCHA));
			modeloAntorcha = glm::translate(modeloAntorcha, glm::vec3(
				-(antorchaMin.x + antorchaMax.x) * 0.5f,
				-antorchaMin.y,
				-(antorchaMin.z + antorchaMax.z) * 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloAntorcha));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaAntorcha ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 1);
			glUniform1i(glGetUniformLocation(programaArbol, "esGlowstone"), 0);
			glBindTexture(GL_TEXTURE_2D, texturaAntorcha);
			glBindVertexArray(antorcha.vao);
			glDrawArrays(GL_TRIANGLES, 0, antorcha.cantidad);
			glBindVertexArray(0);

			// Segunda antorcha: reutiliza exactamente la misma malla y textura.
			glm::mat4 modeloAntorcha2(1.0f);
			modeloAntorcha2 = glm::translate(modeloAntorcha2, glm::vec3(
				POSICION_ANTORCHA_2.x, alturaPiso + POSICION_ANTORCHA_2.y, POSICION_ANTORCHA_2.z));
			modeloAntorcha2 = glm::rotate(modeloAntorcha2, glm::radians(ROTACION_ANTORCHA_2.x), glm::vec3(1, 0, 0));
			modeloAntorcha2 = glm::rotate(modeloAntorcha2, glm::radians(ROTACION_ANTORCHA_2.y), glm::vec3(0, 1, 0));
			modeloAntorcha2 = glm::rotate(modeloAntorcha2, glm::radians(ROTACION_ANTORCHA_2.z), glm::vec3(0, 0, 1));
			modeloAntorcha2 = glm::scale(modeloAntorcha2, glm::vec3(escalaNormalAntorcha * ESCALA_ANTORCHA_2));
			modeloAntorcha2 = glm::translate(modeloAntorcha2, glm::vec3(
				-(antorchaMin.x + antorchaMax.x) * 0.5f,
				-antorchaMin.y,
				-(antorchaMin.z + antorchaMax.z) * 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloAntorcha2));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaAntorcha ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 1);
			glUniform1i(glGetUniformLocation(programaArbol, "esGlowstone"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glBindTexture(GL_TEXTURE_2D, texturaAntorcha);
			glBindVertexArray(antorcha.vao);
			glDrawArrays(GL_TRIANGLES, 0, antorcha.cantidad);
			glBindVertexArray(0);

			// Bloque de glowstone luminoso.
			glm::mat4 modeloGlowstone(1.0f);
			modeloGlowstone = glm::translate(modeloGlowstone, glm::vec3(
				POSICION_GLOWSTONE.x,
				alturaPiso + POSICION_GLOWSTONE.y + 0.5f * ESCALA_GLOWSTONE,
				POSICION_GLOWSTONE.z));
			modeloGlowstone = glm::rotate(modeloGlowstone, glm::radians(ROTACION_GLOWSTONE.x), glm::vec3(1, 0, 0));
			modeloGlowstone = glm::rotate(modeloGlowstone, glm::radians(ROTACION_GLOWSTONE.y), glm::vec3(0, 1, 0));
			modeloGlowstone = glm::rotate(modeloGlowstone, glm::radians(ROTACION_GLOWSTONE.z), glm::vec3(0, 0, 1));
			modeloGlowstone = glm::scale(modeloGlowstone, glm::vec3(ESCALA_GLOWSTONE));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloGlowstone));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaGlowstone ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esGlowstone"), 1);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glBindTexture(GL_TEXTURE_2D, texturaGlowstone);
			glBindVertexArray(glowstone.vao);
			glDrawArrays(GL_TRIANGLES, 0, glowstone.cantidad);
			glBindVertexArray(0);
			glUniform1i(glGetUniformLocation(programaArbol, "esGlowstone"), 0);

			// Portal del Nether al fondo del entorno.
			glm::mat4 modeloPortal(1.0f);
			modeloPortal = glm::translate(modeloPortal, glm::vec3(
				POSICION_PORTAL.x, alturaPiso + POSICION_PORTAL.y, POSICION_PORTAL.z));
			modeloPortal = glm::rotate(modeloPortal, glm::radians(ROTACION_PORTAL.x), glm::vec3(1, 0, 0));
			modeloPortal = glm::rotate(modeloPortal, glm::radians(ROTACION_PORTAL.y), glm::vec3(0, 1, 0));
			modeloPortal = glm::rotate(modeloPortal, glm::radians(ROTACION_PORTAL.z), glm::vec3(0, 0, 1));
			modeloPortal = glm::scale(modeloPortal, glm::vec3(escalaNormalPortal * ESCALA_PORTAL));
			modeloPortal = glm::translate(modeloPortal, glm::vec3(
				-(portalMin.x + portalMax.x) * 0.5f,
				-portalMin.y,
				-(portalMin.z + portalMax.z) * 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloPortal));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaPortal ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 1);
			glBindTexture(GL_TEXTURE_2D, texturaPortal);
			glBindVertexArray(portal.vao);
			glDrawArrays(GL_TRIANGLES, 0, portal.cantidad);
			glBindVertexArray(0);

			// Luna como bloque 3D real, colocada arriba y detras del entorno.
			glm::mat4 modeloLuna(1.0f);
			modeloLuna = glm::translate(modeloLuna, glm::vec3(
				POSICION_LUNA.x, alturaPiso + POSICION_LUNA.y, POSICION_LUNA.z));
			modeloLuna = glm::rotate(modeloLuna, glm::radians(ROTACION_LUNA.x), glm::vec3(1, 0, 0));
			modeloLuna = glm::rotate(modeloLuna, glm::radians(ROTACION_LUNA.y), glm::vec3(0, 1, 0));
			modeloLuna = glm::rotate(modeloLuna, glm::radians(ROTACION_LUNA.z), glm::vec3(0, 0, 1));
			modeloLuna = glm::scale(modeloLuna, glm::vec3(ESCALA_LUNA));
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(modeloLuna));
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), texturaLuna ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLuna"), 1);
			glBindTexture(GL_TEXTURE_2D, texturaLuna);
			glBindVertexArray(luna.vao);
			glDrawArrays(GL_TRIANGLES, 0, luna.cantidad);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// Las hojas se dibujan al final y permanecen completamente opacas.
		if (programaArbol && arbol.hojas.vao) {
			glUseProgram(programaArbol);
			// La antorcha cambio el uniform model; restaurar la matriz del arbol
			// antes de dibujar las hojas evita que aparezcan encima de ella.
			glUniformMatrix4fv(glGetUniformLocation(programaArbol, "model"), 1, GL_FALSE,
				glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(programaArbol, "esAntorcha"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esPortal"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esLuna"), 0);
			glUniform1i(glGetUniformLocation(programaArbol, "texturaCorrecta"), arbol.textura ? 1 : 0);
			glUniform1i(glGetUniformLocation(programaArbol, "esHoja"), 1);
			glUniform1f(glGetUniformLocation(programaArbol, "opacidad"), 1.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, arbol.textura);
			glBindVertexArray(arbol.hojas.vao);
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDrawArrays(GL_TRIANGLES, 0, arbol.hojas.cantidad);
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
	if (programaCielo) glDeleteProgram(programaCielo);
	if (vaoCielo) glDeleteVertexArrays(1, &vaoCielo);
	if (arbol.textura) glDeleteTextures(1, &arbol.textura);
	if (texturaPasto) glDeleteTextures(1, &texturaPasto);
	if (texturaAntorcha) glDeleteTextures(1, &texturaAntorcha);
	if (texturaPortal) glDeleteTextures(1, &texturaPortal);
	if (texturaGlowstone) glDeleteTextures(1, &texturaGlowstone);
	if (texturaLava) glDeleteTextures(1, &texturaLava);
	if (texturaLuna) glDeleteTextures(1, &texturaLuna);
	if (arbol.tronco.vao) glDeleteVertexArrays(1, &arbol.tronco.vao);
	if (arbol.tronco.vbo) glDeleteBuffers(1, &arbol.tronco.vbo);
	if (arbol.hojas.vao) glDeleteVertexArrays(1, &arbol.hojas.vao);
	if (arbol.hojas.vbo) glDeleteBuffers(1, &arbol.hojas.vbo);
	if (pisoMinecraft.vao) glDeleteVertexArrays(1, &pisoMinecraft.vao);
	if (pisoMinecraft.vbo) glDeleteBuffers(1, &pisoMinecraft.vbo);
	if (antorcha.vao) glDeleteVertexArrays(1, &antorcha.vao);
	if (antorcha.vbo) glDeleteBuffers(1, &antorcha.vbo);
	if (portal.vao) glDeleteVertexArrays(1, &portal.vao);
	if (portal.vbo) glDeleteBuffers(1, &portal.vbo);
	if (glowstone.vao) glDeleteVertexArrays(1, &glowstone.vao);
	if (glowstone.vbo) glDeleteBuffers(1, &glowstone.vbo);
	if (luna.vao) glDeleteVertexArrays(1, &luna.vao);
	if (luna.vbo) glDeleteBuffers(1, &luna.vbo);

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

	// Las hojas deben permanecer opacas durante toda la practica.
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
