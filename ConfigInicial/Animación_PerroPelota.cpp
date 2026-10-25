// Practica #10
// Bello Zaragoza Demian
// Fecha de entrega: 24 de octubre de 2026
// Número de cuenta: 320200928


#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematicsk
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


// Referencia de altura para el modo P (no se dibuja ni emite luz).
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};


//Anim
float rotBall = 0;
bool AnimBall = false; // Giro original, controlado con N.

// Animacion vertical independiente, controlada unicamente con P.
bool AnimVertical = false;
enum ModoAnimacionPelota { MODO_GIRO, MODO_VERTICAL, MODO_CABEZAZOS };
ModoAnimacionPelota modoPelota = MODO_GIRO;
double tiempoVertical = 0.0;
const double PERIODO_VERTICAL = 4.0;
float desplazamientoVertical = 0.0f;
float alturaCentroBall = 0.0f;

// Mide la altura original de la pelota sin moverla ni cambiar su pivote.
// La posicion de partida sigue siendo la que tiene ball.obj en el material.
static bool MedirAlturaPelota(const char* ruta, float& centroY) {
	std::ifstream archivo(ruta);
	if (!archivo) return false;
	float minimo = (std::numeric_limits<float>::max)();
	float maximo = -minimo;
	bool encontrado = false;
	std::string linea;
	while (std::getline(archivo, linea)) {
		std::istringstream datos(linea);
		std::string tipo;
		float x, y, z;
		if ((datos >> tipo) && tipo == "v" && (datos >> x >> y >> z)) {
			if (y < minimo) minimo = y;
			if (y > maximo) maximo = y;
			encontrado = true;
		}
	}
	if (encontrado) centroY = (minimo + maximo) * 0.5f;
	return encontrado;
}


// NUEVA ANIMACION: B inicia/pausa/reanuda; N y P conservan sus modos.
// Sentidos definidos mirando la escena desde arriba (+Y).
const double PI_ANIM = 3.14159265358979323846;
const double SEGUNDOS_VUELTA = 8.0;
const float RADIO_EN_ALTURAS = 1.35f;
const float SALTO_EN_ALTURAS = 0.32f;
const float SEPARACION_PELOTA_EN_ALTURAS = 0.65f;

// Ajustes manuales del contacto, en unidades relativas a la altura del perro.
// La referencia inicial es el centro inferior de ball.obj, junto a la nariz.
const glm::vec3 AJUSTE_NARIZ(0.0f, 0.0f, 0.0f);
const float AJUSTE_FRENTE_GRADOS = 0.0f; // Solo si el OBJ no mira hacia +Z.
bool AnimCabezas = false;
double tiempoCabezas = 0.0;
float radioRecorrido = 1.0f, alturaPerro = 1.0f, radioPelotaY = 0.1f;
float alturaSalto = 0.0f, alturaContacto = 0.0f;
glm::vec3 centroPelotaOriginal(0.0f), narizOriginal(0.0f), centroRecorrido(0.0f);
glm::mat4 matrizPerroAnimada(1.0f), matrizPelotaAnimada(1.0f);

static bool MedirCajaOBJ(const char* ruta, glm::vec3& minimo, glm::vec3& maximo) {
	std::ifstream archivo(ruta);
	if (!archivo) return false;
	const float limite = (std::numeric_limits<float>::max)();
	minimo = glm::vec3(limite); maximo = glm::vec3(-limite);
	bool encontrado = false;
	std::string linea, tipo;
	while (std::getline(archivo, linea)) {
		std::istringstream datos(linea);
		glm::vec3 p;
		if ((datos >> tipo) && tipo == "v" && (datos >> p.x >> p.y >> p.z)) {
			minimo = glm::min(minimo, p); maximo = glm::max(maximo, p);
			encontrado = true;
		}
	}
	return encontrado;
}

static void ActualizarCabezas() {
	const double fase = 2.0 * PI_ANIM * tiempoCabezas / SEGUNDOS_VUELTA;
	// Velocidad variable y siempre positiva: desaceleran cerca del contacto.
	const double angulo = fase - 0.10 * std::sin(4.0 * fase);
	const double anguloPerro = angulo;
	const double anguloPelota = PI_ANIM - angulo;
	const double intervaloGolpes = SEGUNDOS_VUELTA * 0.5;
	const double primerGolpe = SEGUNDOS_VUELTA * 0.25;
	const double respectoGolpe = std::remainder(
		tiempoCabezas - primerGolpe, intervaloGolpes);
	// Salto breve con trayectoria parabolica: despegue, apice y aterrizaje.
	const double medioSalto = 0.48;
	const double u = respectoGolpe / medioSalto;
	const float salto = (std::abs(u) < 1.0)
		? alturaSalto * static_cast<float>(1.0 - u * u) : 0.0f;
	// Pelota: tras el golpe sube, pierde velocidad y vuelve a caer.
	double desdeGolpe = std::fmod(tiempoCabezas - primerGolpe, intervaloGolpes);
	if (desdeGolpe < 0.0) desdeGolpe += intervaloGolpes;
	const float q = static_cast<float>(desdeGolpe / intervaloGolpes);
	const float arco = 4.0f * q * (1.0f - q);
	const float yPelota = alturaContacto + radioPelotaY + alturaSalto
		+ alturaPerro * SEPARACION_PELOTA_EN_ALTURAS * arco;
	glm::vec3 posicionPelota(
		centroRecorrido.x + radioRecorrido * static_cast<float>(std::cos(anguloPelota)),
		yPelota,
		centroRecorrido.z + radioRecorrido * static_cast<float>(std::sin(anguloPelota)));

	matrizPerroAnimada = glm::translate(glm::mat4(1.0f), centroRecorrido);
	matrizPerroAnimada = glm::rotate(matrizPerroAnimada,
		static_cast<float>(-anguloPerro), glm::vec3(0, 1, 0));
	matrizPerroAnimada = glm::translate(matrizPerroAnimada,
		glm::vec3(radioRecorrido, salto, 0.0f));
	matrizPerroAnimada = glm::rotate(matrizPerroAnimada,
		glm::radians(AJUSTE_FRENTE_GRADOS), glm::vec3(0, 1, 0));
	matrizPerroAnimada = glm::translate(matrizPerroAnimada,
		glm::vec3(-narizOriginal.x, 0.0f, -narizOriginal.z));

	// Quitar el desplazamiento interno del OBJ para que su centro siga la orbita.
	matrizPelotaAnimada = glm::translate(glm::mat4(1.0f), posicionPelota);
	matrizPelotaAnimada = glm::rotate(matrizPelotaAnimada,
		static_cast<float>(angulo), glm::vec3(0, 1, 0));
	matrizPelotaAnimada = glm::translate(matrizPelotaAnimada, -centroPelotaOriginal);
}

static bool PrepararCabezas() {
	glm::vec3 dMin, dMax, bMin, bMax, pMin, pMax;
	if (!MedirCajaOBJ("Models/RedDog.obj", dMin, dMax) ||
		!MedirCajaOBJ("Models/ball.obj", bMin, bMax)) return false;
	alturaPerro = dMax.y - dMin.y;
	if (alturaPerro < 0.0001f || bMax.y - bMin.y < 0.0001f) return false;
	centroPelotaOriginal = (bMin + bMax) * 0.5f;
	radioPelotaY = (bMax.y - bMin.y) * 0.5f;
	narizOriginal = glm::vec3(centroPelotaOriginal.x, bMin.y,
		centroPelotaOriginal.z) + AJUSTE_NARIZ * alturaPerro;
	alturaContacto = narizOriginal.y;
	alturaSalto = alturaPerro * SALTO_EN_ALTURAS;
	centroRecorrido = glm::vec3((dMin.x + dMax.x) * 0.5f, 0.0f,
		(dMin.z + dMax.z) * 0.5f);
	radioRecorrido = alturaPerro * RADIO_EN_ALTURAS;
	// Centrar el recorrido en el piso y reservar margen para el cuerpo del perro.
	if (MedirCajaOBJ("Models/piso.obj", pMin, pMax)) {
		centroRecorrido.x = (pMin.x + pMax.x) * 0.5f;
		centroRecorrido.z = (pMin.z + pMax.z) * 0.5f;
		float alcanceX = glm::max(std::abs(dMin.x - narizOriginal.x), std::abs(dMax.x - narizOriginal.x));
		float alcanceZ = glm::max(std::abs(dMin.z - narizOriginal.z), std::abs(dMax.z - narizOriginal.z));
		float margen = std::sqrt(alcanceX * alcanceX + alcanceZ * alcanceZ);
		float disponible = 0.5f * glm::min(pMax.x - pMin.x, pMax.z - pMin.z) - margen;
		if (disponible > alturaPerro * 0.3f)
			radioRecorrido = glm::min(radioRecorrido, disponible * 0.9f);
		else std::cerr << "Aviso: piso pequeno para el recorrido; revisa RADIO_EN_ALTURAS.\n";
	}
	ActualizarCabezas();
	return true;
}

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 10 Bello Zaragoza Demian", nullptr, nullptr);

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


	//models
	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Ball((char*)"Models/ball.obj");
	if (!MedirAlturaPelota("Models/ball.obj", alturaCentroBall)) {
		std::cerr << "No se pudo medir Models/ball.obj. Revisa la ruta." << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}
	if (!PrepararCabezas()) {
		std::cerr << "No se pudieron medir RedDog.obj y ball.obj. Revisa Models.\n";
		glfwDestroyWindow(window); glfwTerminate(); return EXIT_FAILURE;
	}
	// Encuadre inicial mas amplio para ver el recorrido completo.
	camera = Camera(glm::vec3(centroRecorrido.x,
		alturaContacto + alturaPerro * 0.5f,
		centroRecorrido.z + radioRecorrido * 3.0f + alturaPerro * 3.0f));
	std::cout << "B: perro y pelota con cabezazos (iniciar/pausar/reanudar)\n"
		<< "N: giro original | P: subida y bajada\n";



	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	lastFrame = static_cast<GLfloat>(glfwGetTime());

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
		Animation();

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


		// Sin luz puntual ni linterna azul. Solo iluminacion general estable.
		// Se dejan atenuaciones validas para evitar divisiones por cero en shaders.
		for (int i = 0; i < 4; ++i) {
			const std::string base = "pointLights[" + std::to_string(i) + "].";
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "position").c_str()), 0.0f, 10.0f, 0.0f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()), 0, 0, 0);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()), 0, 0, 0);
			glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()), 0, 0, 0);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "linear").c_str()), 0.0f);
			glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "quadratic").c_str()), 0.0f);
		}
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), 0, 10, 0);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), 0, -1, 0);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0, 0, 0);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0, 0, 0);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0, 0, 0);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), 0.98f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), 0.95f);

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



		//Carga de modelo 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		model = (modoPelota == MODO_CABEZAZOS) ? matrizPerroAnimada : glm::mat4(1.0f);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		Dog.Draw(lightingShader);

		model = glm::mat4(1);
		glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		// T * R: se agrega altura y se conserva exactamente el giro original.
		model = glm::translate(model, glm::vec3(0.0f, desplazamientoVertical, 0.0f));
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
		if (modoPelota == MODO_CABEZAZOS) model = matrizPelotaAnimada;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);
		glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glBindVertexArray(0);


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


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

	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		AnimBall = false; AnimVertical = false;
		rotBall = 0.0f; desplazamientoVertical = 0.0f; tiempoVertical = 0.0;
		if (modoPelota != MODO_CABEZAZOS) {
			modoPelota = MODO_CABEZAZOS;
			tiempoCabezas = 0.0;
			AnimCabezas = true;
			ActualizarCabezas();
		}
		else {
			AnimCabezas = !AnimCabezas;
		}
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		AnimCabezas = false;
		AnimBall = false;
		rotBall = 0.0f; // Volver a la alineacion original con la nariz.
		if (modoPelota != MODO_VERTICAL) {
			modoPelota = MODO_VERTICAL;
			tiempoVertical = 0.0;
			desplazamientoVertical = 0.0f;
			AnimVertical = true;
		}
		else {
			AnimVertical = !AnimVertical;
		}
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		AnimCabezas = false;
		AnimVertical = false;
		desplazamientoVertical = 0.0f;
		tiempoVertical = 0.0;
		if (modoPelota != MODO_GIRO) {
			modoPelota = MODO_GIRO;
			rotBall = 0.0f;
			AnimBall = true;
		}
		else {
			AnimBall = !AnimBall;
		}
	}
}
void Animation() {
	if (modoPelota == MODO_CABEZAZOS) {
		if (AnimCabezas) {
			tiempoCabezas = std::fmod(tiempoCabezas + static_cast<double>(deltaTime), SEGUNDOS_VUELTA);
			ActualizarCabezas();
		}
		return; // No mezclar este recorrido con las animaciones anteriores.
	}
	// Al pausar no cambia ni el tiempo acumulado ni el desplazamiento.
	// P reanuda desde ese mismo punto, sin reiniciar ni saltar por el tiempo pausado.
	if (AnimVertical) {
		tiempoVertical += static_cast<double>(deltaTime);
		const double fase = std::fmod(tiempoVertical, PERIODO_VERTICAL) / PERIODO_VERTICAL;
		const float avance = static_cast<float>(0.5 - 0.5 * std::cos(6.283185307179586 * fase));
		desplazamientoVertical = (pointLightPositions[0].y - alturaCentroBall) * avance;
	}
	// Se conserva la animacion original de N, incluido su incremento.
	if (AnimBall)
	{
		rotBall += 0.2f;
		//printf("%f", rotBall);
	}
	else
	{
		//rotBall = 0.0f;
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
