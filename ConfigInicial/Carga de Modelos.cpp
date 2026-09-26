// Practica #6 
// Bello Zaragoza Demian
// Fecha de entrega: 26 de septiembre 2026
// Número de cuenta: 320200928

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <random>
#include <cstddef>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLuint WIDTH = 1000, HEIGHT = 700;
int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 700;
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024] = { false };
bool firstMouse = true, mostrarPasto = true;
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
GLfloat deltaTime = 0.0f, lastFrame = 0.0f;

// EDITAR OBJETOS: busca "ACOMODA TODOS LOS ELEMENTOS AQUI" dentro de main().
// Las posiciones son coordenadas reales de la escena, sin ajuste automatico al piso.
// Las escalas son factores reales del OBJ: no se multiplican por una escala oculta.

// COLINA: forma del terreno ANTES de mover o girar el paisaje completo.
// X/Z: centro horizontal, en unidades del paisaje (referencia original del perro).
// Y: elevacion adicional del pico. 0 mantiene la altura original de 3 unidades.
const glm::vec3 COLINA_POSICION(0.0f, 0.0f, -9.0f);
const glm::vec3 COLINA_ESCALA(1.0f, 1.0f, 1.0f);
const float COLINA_GIRO_Y = 0.0f; // Grados, gira su base ovalada.
const float ALTURA_PASTO = 0.085f; // Altura base; algunas matas son mas largas.
const int CANTIDAD_MATAS = 72000; // El doble del pasto original.
const float CANTIDAD_NUBES_CIELO = 0.57f; // 0.0: cielo claro; 1.0: mas nuboso.
glm::mat4 matrizPaisaje(1.0f); // El pasto acompana al piso.

float unidad = 1.0f, pisoY = -1.0f;
float centroX = 0.0f, centroZ = 0.0f;
glm::vec3 perroMin, perroMax;
// Limites transformados: el hueco del pasto y la sombra siguen al modelo.
glm::vec3 perroMundoMin, perroMundoMax, llamaMundoMin, llamaMundoMax;
glm::vec3 cofreMundoMin, cofreMundoMax, municionMundoMin, municionMundoMax;
glm::vec3 bigPotMundoMin, bigPotMundoMax;
glm::vec3 arbustoAtrasMundoMin, arbustoAtrasMundoMax;
glm::vec3 arbustoDelanteMundoMin, arbustoDelanteMundoMax;
glm::vec3 rocaMundoMin[13], rocaMundoMax[13];

void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseCallback(GLFWwindow*, double, double);
void DoMovement();

// Lee solo los vertices del OBJ para colocar el piso bajo su punto inferior.
// No modifica ni traslada el modelo del perrito.
void MedirPerro(const char* ruta)
{
    std::ifstream archivo(ruta);
    if (!archivo) throw std::runtime_error("No se pudo abrir Models/RedDog.obj. Revisa el directorio de trabajo.");
    float limite = (std::numeric_limits<float>::max)();
    perroMin = glm::vec3(limite);
    perroMax = glm::vec3(-limite);
    std::string linea, tipo;
    bool encontrado = false;
    while (std::getline(archivo, linea))
    {
        std::istringstream datos(linea);
        datos >> tipo;
        glm::vec3 p;
        if (tipo == "v" && (datos >> p.x >> p.y >> p.z))
        {
            perroMin = glm::min(perroMin, p);
            perroMax = glm::max(perroMax, p);
            encontrado = true;
        }
    }
    if (!encontrado) throw std::runtime_error("El OBJ no contiene vertices legibles.");
    glm::vec3 tam = perroMax - perroMin;
    unidad = glm::max(tam.x, glm::max(tam.y, tam.z));
    if (unidad < 0.0001f) throw std::runtime_error("El modelo tiene dimensiones invalidas.");
    pisoY = perroMin.y;
    centroX = (perroMin.x + perroMax.x) * 0.5f;
    centroZ = (perroMin.z + perroMax.z) * 0.5f;
}

// Medir el cofre al cargarlo: no hay que adivinar su escala ni su origen.
void MedirOBJ(const char* ruta, glm::vec3& minimo, glm::vec3& maximo)
{
    std::ifstream archivo(ruta);
    if (!archivo)
        throw std::runtime_error(std::string("No se pudo abrir: ") + ruta);
    float limite = (std::numeric_limits<float>::max)();
    minimo = glm::vec3(limite);
    maximo = glm::vec3(-limite);
    bool encontrado = false;
    std::string linea;
    while (std::getline(archivo, linea))
    {
        std::istringstream datos(linea);
        std::string tipo;
        glm::vec3 v;
        if ((datos >> tipo) && tipo == "v" && (datos >> v.x >> v.y >> v.z))
        {
            if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z))
                throw std::runtime_error("El OBJ contiene coordenadas invalidas.");
            minimo = glm::min(minimo, v);
            maximo = glm::max(maximo, v);
            encontrado = true;
        }
    }
    if (!encontrado || maximo.y - minimo.y <= 0.000001f)
        throw std::runtime_error(std::string("OBJ sin altura valida: ") + ruta);
}

float Suavizar(float a, float b, float x)
{
    float t = glm::clamp((x - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float Campana(float x, float z, float cx, float cz, float anchoX, float anchoZ)
{
    float dx = (x - cx) / anchoX;
    float dz = (z - cz) / anchoZ;
    return std::exp(-(dx * dx + dz * dz));
}

float AlturaTerreno(float x, float z)
{
    float px = (x - centroX) / unidad;
    float pz = (z - centroZ) / unidad;
    // La transicion se desplaza con la colina: ya no queda anclada al fondo original.
    float mezcla = Suavizar(1.5f, 5.5f, COLINA_POSICION.z + 9.0f - pz);
    float dx = px - COLINA_POSICION.x;
    float dz = pz - COLINA_POSICION.z;
    float angulo = glm::radians(COLINA_GIRO_Y);
    float localX = std::cos(angulo) * dx - std::sin(angulo) * dz;
    float localZ = std::sin(angulo) * dx + std::cos(angulo) * dz;
    float colina = (3.0f * COLINA_ESCALA.y + COLINA_POSICION.y) *
        Campana(localX, localZ, 0.0f, 0.0f,
            glm::max(std::abs(10.0f * COLINA_ESCALA.x), 0.001f),
            glm::max(std::abs(5.0f * COLINA_ESCALA.z), 0.001f));
    return pisoY + unidad * mezcla * colina;
}

// Posicion ABSOLUTA. Ninguna coordenada se ajusta al terreno ni al perro.
// Pivote: punto local alrededor del que gira el objeto (centro de su base).
glm::mat4 TransformarModelo(const glm::vec3& posicion,
    const glm::vec3& rotacion, const glm::vec3& escala, const glm::vec3& pivote)
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, posicion);
    m = glm::rotate(m, glm::radians(rotacion.z), glm::vec3(0, 0, 1));
    m = glm::rotate(m, glm::radians(rotacion.y), glm::vec3(0, 1, 0));
    m = glm::rotate(m, glm::radians(rotacion.x), glm::vec3(1, 0, 0));
    m = glm::scale(m, escala);
    return glm::translate(m, -pivote);
}

void LimitesTransformados(const glm::mat4& m, const glm::vec3& minimo,
    const glm::vec3& maximo, glm::vec3& salidaMin, glm::vec3& salidaMax)
{
    float limite = (std::numeric_limits<float>::max)();
    salidaMin = glm::vec3(limite);
    salidaMax = glm::vec3(-limite);
    for (int i = 0; i < 8; ++i)
    {
        glm::vec3 p((i & 1) ? maximo.x : minimo.x,
            (i & 2) ? maximo.y : minimo.y,
            (i & 4) ? maximo.z : minimo.z);
        glm::vec3 mundo = glm::vec3(m * glm::vec4(p, 1.0f));
        salidaMin = glm::min(salidaMin, mundo);
        salidaMax = glm::max(salidaMax, mundo);
    }
}

bool BajoModelo(float x, float z, const glm::vec3& minimo, const glm::vec3& maximo)
{
    float margen = 0.04f * unidad;
    glm::vec3 punto = glm::vec3(matrizPaisaje * glm::vec4(x, AlturaTerreno(x, z), z, 1.0f));
    if (minimo.y > punto.y + ALTURA_PASTO * unidad * 1.5f)
        return false;
    return punto.x > minimo.x - margen && punto.x < maximo.x + margen &&
        punto.z > minimo.z - margen && punto.z < maximo.z + margen;
}

glm::vec3 NormalTerreno(float x, float z)
{
    float e = unidad * 0.025f;
    return glm::normalize(glm::vec3(
        AlturaTerreno(x - e, z) - AlturaTerreno(x + e, z),
        2.0f * e,
        AlturaTerreno(x, z - e) - AlturaTerreno(x, z + e)));
}

struct VerticePaisaje
{
    glm::vec3 posicion;
    glm::vec3 normal;
    glm::vec3 color;
    float flexion;
};

struct MallaPaisaje
{
    GLuint vao = 0, vbo = 0;
    GLsizei cantidad = 0;

    void Subir(const std::vector<VerticePaisaje>& vertices)
    {
        cantidad = static_cast<GLsizei>(vertices.size());
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VerticePaisaje), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticePaisaje), reinterpret_cast<void*>(offsetof(VerticePaisaje, posicion)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VerticePaisaje), reinterpret_cast<void*>(offsetof(VerticePaisaje, normal)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VerticePaisaje), reinterpret_cast<void*>(offsetof(VerticePaisaje, color)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VerticePaisaje), reinterpret_cast<void*>(offsetof(VerticePaisaje, flexion)));
        glBindVertexArray(0);
    }

    void Dibujar() const
    {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, cantidad);
        glBindVertexArray(0);
    }

    void Liberar()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
};

void CrearTerreno(MallaPaisaje& malla)
{
    const int NX = 160, NZ = 150;
    std::vector<VerticePaisaje> vertices;
    vertices.reserve(NX * NZ * 6);
    auto punto = [](float px, float pz) -> VerticePaisaje
        {
            float x = centroX + px * unidad, z = centroZ + pz * unidad;
            float variacion = 0.5f + 0.25f * std::sin(px * 1.7f) * std::cos(pz * 1.3f)
                + 0.15f * std::sin(px * 4.2f + pz * 2.8f);
            glm::vec3 color = glm::mix(glm::vec3(0.24f, 0.43f, 0.055f),
                glm::vec3(0.58f, 0.72f, 0.13f), variacion);
            return { glm::vec3(x, AlturaTerreno(x, z), z), NormalTerreno(x, z), color, 0.0f };
        };
    for (int z = 0; z < NZ; ++z)
        for (int x = 0; x < NX; ++x)
        {
            float x0 = -14.0f + 28.0f * x / NX;
            float x1 = -14.0f + 28.0f * (x + 1) / NX;
            float z0 = -19.0f + 28.0f * z / NZ;
            float z1 = -19.0f + 28.0f * (z + 1) / NZ;
            VerticePaisaje a = punto(x0, z0), b = punto(x0, z1);
            VerticePaisaje c = punto(x1, z0), d = punto(x1, z1);
            vertices.push_back(a); vertices.push_back(b); vertices.push_back(c);
            vertices.push_back(c); vertices.push_back(b); vertices.push_back(d);
        }
    malla.Subir(vertices);
}

void CrearArbolDesdeOBJ(MallaPaisaje& malla, const char* ruta,
    const glm::vec3& minimo, const glm::vec3& maximo)
{
    std::ifstream archivo(ruta);
    if (!archivo) throw std::runtime_error(std::string("No se pudo abrir: ") + ruta);
    std::vector<glm::vec3> posiciones;
    std::vector<VerticePaisaje> vertices;
    std::string linea;
    float alto = glm::max(maximo.y - minimo.y, 0.0001f);
    glm::vec2 centro((minimo.x + maximo.x) * 0.5f, (minimo.z + maximo.z) * 0.5f);
    float radio = glm::max(maximo.x - minimo.x, maximo.z - minimo.z) * 0.5f;
    while (std::getline(archivo, linea))
    {
        if (linea.size() < 3) continue;
        if (linea[0] == 'v' && linea[1] == ' ')
        {
            std::istringstream entrada(linea.substr(2));
            glm::vec3 p;
            if (entrada >> p.x >> p.y >> p.z) posiciones.push_back(p);
        }
        else if (linea[0] == 'f' && linea[1] == ' ')
        {
            std::istringstream entrada(linea.substr(2));

            std::string tokenA, tokenB, tokenC;

            if (!(entrada >> tokenA >> tokenB >> tokenC))
                throw std::runtime_error("Cara incompleta en el OBJ del arbol");

            auto obtenerIndiceVertice = [](const std::string& token) -> int
                {
                    // Admite:
                    // 123
                    // 123/45
                    // 123//67
                    // 123/45/67

                    size_t barra = token.find('/');

                    std::string numero =
                        (barra == std::string::npos)
                        ? token
                        : token.substr(0, barra);

                    if (numero.empty())
                        throw std::runtime_error(
                            "Indice de vertice invalido en OBJ");

                    return std::stoi(numero);
                };

            int a = obtenerIndiceVertice(tokenA);
            int b = obtenerIndiceVertice(tokenB);
            int c = obtenerIndiceVertice(tokenC);

            if (a < 1 || b < 1 || c < 1 ||
                a > static_cast<int>(posiciones.size()) ||
                b > static_cast<int>(posiciones.size()) ||
                c > static_cast<int>(posiciones.size()))
            {
                throw std::runtime_error(
                    "Indices fuera de rango en el OBJ del arbol");
            }

            glm::vec3 puntos[] =
            {
                posiciones[a - 1],
                posiciones[b - 1],
                posiciones[c - 1]
            };

            glm::vec3 cruz =
                glm::cross(
                    puntos[1] - puntos[0],
                    puntos[2] - puntos[0]
                );

            float longitud = glm::length(cruz);

            if (longitud < 0.000001f)
                continue;

            glm::vec3 normal = cruz / longitud;

            for (const glm::vec3& p : puntos)
            {
                float altura =
                    (p.y - minimo.y) / alto;

                float distancia =
                    glm::length(
                        glm::vec2(p.x, p.z) - centro
                    ) / radio;

                float detalle =
                    0.5f +
                    0.5f *
                    std::sin(
                        p.x * 26.0f +
                        p.z * 17.0f +
                        p.y * 19.0f
                    );

                bool tronco =
                    altura < 0.37f ||
                    (altura < 0.72f &&
                        distancia < 0.12f);

                glm::vec3 color;

                if (tronco)
                {
                    color = glm::mix(
                        glm::vec3(0.32f, 0.15f, 0.065f),
                        glm::vec3(0.72f, 0.40f, 0.18f),
                        detalle
                    );
                }
                else
                {
                    color = glm::mix(
                        glm::vec3(0.065f, 0.37f, 0.055f),
                        glm::vec3(0.44f, 0.84f, 0.10f),
                        glm::clamp(
                            0.35f +
                            0.38f * detalle +
                            0.2f * normal.y,
                            0.0f,
                            1.0f
                        )
                    );
                }

                vertices.push_back(
                    { p, normal, color, 0.0f }
                );
            }
        }
    }
    if (vertices.empty()) throw std::runtime_error("El OBJ del arbol no tiene caras");
    malla.Subir(vertices);
}

// Carga Roca1 sin textura y le aplica directamente el color gris del material LSG.
// Admite caras OBJ en formato v, v/vt, v//vn y v/vt/vn.
void CrearRocaColorDesdeOBJ(MallaPaisaje& malla, const char* ruta, const glm::vec3& color)
{
    std::ifstream archivo(ruta);
    if (!archivo)
        throw std::runtime_error(std::string("No se pudo abrir: ") + ruta);

    std::vector<glm::vec3> posiciones;
    std::vector<VerticePaisaje> vertices;
    std::string linea;

    auto obtenerIndiceVertice = [](const std::string& token) -> int
        {
            size_t barra = token.find('/');
            std::string numero =
                (barra == std::string::npos) ? token : token.substr(0, barra);

            if (numero.empty())
                throw std::runtime_error("Indice de vertice invalido en Roca1.obj");

            return std::stoi(numero);
        };

    while (std::getline(archivo, linea))
    {
        if (linea.size() < 2)
            continue;

        if (linea[0] == 'v' && linea.size() > 2 && linea[1] == ' ')
        {
            std::istringstream entrada(linea.substr(2));
            glm::vec3 p;
            if (entrada >> p.x >> p.y >> p.z)
                posiciones.push_back(p);
        }
        else if (linea[0] == 'f' && linea.size() > 2 && linea[1] == ' ')
        {
            std::istringstream entrada(linea.substr(2));
            std::vector<std::string> tokens;
            std::string token;

            while (entrada >> token)
                tokens.push_back(token);

            if (tokens.size() < 3)
                continue;

            // Triangula cualquier cara tipo triangulo, quad o poligono con un fan.
            for (size_t i = 1; i + 1 < tokens.size(); ++i)
            {
                int ia = obtenerIndiceVertice(tokens[0]);
                int ib = obtenerIndiceVertice(tokens[i]);
                int ic = obtenerIndiceVertice(tokens[i + 1]);

                if (ia < 1 || ib < 1 || ic < 1 ||
                    ia > static_cast<int>(posiciones.size()) ||
                    ib > static_cast<int>(posiciones.size()) ||
                    ic > static_cast<int>(posiciones.size()))
                {
                    throw std::runtime_error("Indices fuera de rango en Roca1.obj");
                }

                glm::vec3 p0 = posiciones[ia - 1];
                glm::vec3 p1 = posiciones[ib - 1];
                glm::vec3 p2 = posiciones[ic - 1];

                glm::vec3 cruz = glm::cross(p1 - p0, p2 - p0);
                float longitud = glm::length(cruz);

                if (longitud < 0.000001f)
                    continue;

                glm::vec3 normal = cruz / longitud;

                vertices.push_back({ p0, normal, color, 0.0f });
                vertices.push_back({ p1, normal, color, 0.0f });
                vertices.push_back({ p2, normal, color, 0.0f });
            }
        }
    }

    if (vertices.empty())
        throw std::runtime_error("Roca1.obj no tiene caras validas");

    malla.Subir(vertices);
}

void CrearPasto(MallaPaisaje& malla)
{
    std::mt19937 generador(2026); // Distribucion fija: el pasto no se mueve.
    std::uniform_real_distribution<float> azar(0.0f, 1.0f);
    std::vector<VerticePaisaje> vertices;
    vertices.reserve(CANTIDAD_MATAS * 12);
    for (int i = 0; i < CANTIDAD_MATAS; ++i)
    {
        float px, pz;
        if (i < CANTIDAD_MATAS * 45 / 100)
        {
            // Primer plano: incluye el suelo entre el perro y la camara.
            px = -7.0f + azar(generador) * 14.0f;
            pz = 3.0f + azar(generador) * 5.85f; // Hasta Z=8.85; piso termina en 9.
        }
        else if (i < CANTIDAD_MATAS * 80 / 100)
        {
            // Centro: rodea los modelos dejando sus bases despejadas.
            px = -7.0f + azar(generador) * 14.0f;
            pz = -6.0f + azar(generador) * 9.0f;
        }
        else
        {
            // Fondo y colina: menor densidad por la distancia.
            px = -13.5f + azar(generador) * 27.0f;
            pz = -18.0f + azar(generador) * 21.0f;
        }
        float x = centroX + px * unidad, z = centroZ + pz * unidad;
        if (BajoModelo(x, z, perroMundoMin, perroMundoMax) ||
            BajoModelo(x, z, llamaMundoMin, llamaMundoMax) ||
            BajoModelo(x, z, cofreMundoMin, cofreMundoMax) ||
            BajoModelo(x, z, municionMundoMin, municionMundoMax) ||
            BajoModelo(x, z, bigPotMundoMin, bigPotMundoMax) ||
            BajoModelo(x, z, arbustoAtrasMundoMin, arbustoAtrasMundoMax) ||
            BajoModelo(x, z, arbustoDelanteMundoMin, arbustoDelanteMundoMax)) continue;

        bool dentroRoca = false;
        for (int r = 0; r < 13; ++r)
            if (BajoModelo(x, z, rocaMundoMin[r], rocaMundoMax[r]))
                dentroRoca = true;
        if (dentroRoca) continue;

        float alto = unidad * ALTURA_PASTO * (0.70f + azar(generador) * 0.60f);
        bool mataAlta = (i % 11 == 0); // Brotes mas largos dispersos, estaticos.
        if (mataAlta) alto *= 1.65f;
        float ancho = alto * (0.075f + azar(generador) * 0.05f);
        glm::vec3 base(x, AlturaTerreno(x, z) - 0.006f * unidad, z);
        // Paleta mas viva: verdes claros al sol y verde profundo entre hojas.
        float tono = glm::clamp(0.22f + azar(generador) * 0.68f +
            0.10f * std::sin(px * 0.7f) * std::cos(pz * 0.9f), 0.0f, 1.0f);
        glm::vec3 color = glm::mix(glm::vec3(0.20f, 0.43f, 0.035f),
            glm::vec3(0.68f, 0.82f, 0.13f), tono);
        float angulo = azar(generador) * 6.2831853f;
        int hojas = mataAlta ? 5 : 3;
        for (int hoja = 0; hoja < hojas; ++hoja)
        {
            float a = angulo + hoja * (6.2831853f / hojas);
            glm::vec3 lado(std::cos(a), 0.0f, std::sin(a));
            float largoHoja = alto * (0.85f + azar(generador) * 0.30f);
            glm::vec3 punta = base + glm::vec3(0.0f, largoHoja, 0.0f)
                + lado * largoHoja * (0.16f + azar(generador) * 0.18f);
            glm::vec3 n(0.0f, 1.0f, 0.0f);
            vertices.push_back({ base - lado * ancho, n, color * 0.73f, 0.0f });
            vertices.push_back({ base + lado * ancho, n, color * 0.73f, 0.0f });
            vertices.push_back({ punta, n, color, 1.0f });
        }
    }
    malla.Subir(vertices);
}


GLuint Compilar(GLenum tipo, const char* codigo);

// -----------------------------------------------------------------------------
// BRILLO SUAVE PARA BIGPOT
// Disco semitransparente dorado sobre el suelo, similar al resplandor del cofre.
// -----------------------------------------------------------------------------
void CrearDiscoBrillo(MallaPaisaje& malla)
{
    std::vector<VerticePaisaje> vertices;
    const int segmentos = 64;
    const float pi = 3.14159265f;

    glm::vec3 centro(0.0f, 0.0f, 0.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    glm::vec3 color(1.0f, 0.56f, 0.08f);

    for (int i = 0; i < segmentos; ++i)
    {
        float a0 = 2.0f * pi * i / segmentos;
        float a1 = 2.0f * pi * (i + 1) / segmentos;

        glm::vec3 p0(std::cos(a0), 0.0f, std::sin(a0));
        glm::vec3 p1(std::cos(a1), 0.0f, std::sin(a1));

        // flexion se reutiliza aqui como intensidad:
        // 1 en el centro, 0 en el borde.
        vertices.push_back({ centro, normal, color, 1.0f });
        vertices.push_back({ p0, normal, color, 0.0f });
        vertices.push_back({ p1, normal, color, 0.0f });
    }

    malla.Subir(vertices);
}

const char* brilloVS = R"GLSL(
#version 330 core
layout(location=0) in vec3 posicion;
layout(location=2) in vec3 color;
layout(location=3) in float intensidad;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vColor;
out float vIntensidad;

void main()
{
    vColor = color;
    vIntensidad = intensidad;
    gl_Position = projection * view * model * vec4(posicion, 1.0);
}
)GLSL";

const char* brilloFS = R"GLSL(
#version 330 core
in vec3 vColor;
in float vIntensidad;

out vec4 fragColor;

void main()
{
    float a = pow(clamp(vIntensidad, 0.0, 1.0), 1.7) * 0.46;
    fragColor = vec4(vColor, a);
}
)GLSL";

GLuint CrearProgramaBrillo()
{
    GLuint vs = Compilar(GL_VERTEX_SHADER, brilloVS);
    GLuint fs = Compilar(GL_FRAGMENT_SHADER, brilloFS);

    GLuint programa = glCreateProgram();
    glAttachShader(programa, vs);
    glAttachShader(programa, fs);
    glLinkProgram(programa);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = GL_FALSE;
    glGetProgramiv(programa, GL_LINK_STATUS, &ok);

    if (!ok)
    {
        char log[4096] = {};
        glGetProgramInfoLog(programa, sizeof(log), nullptr, log);
        glDeleteProgram(programa);
        throw std::runtime_error(std::string("Error enlace brillo: ") + log);
    }

    return programa;
}

// Shaders exclusivos del paisaje; no se modifican tus shaders del perro.
const char* paisajeVS = R"GLSL(
#version 330 core
layout(location=0) in vec3 posicion;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float flexion;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float unidad;
out vec3 vPos;
out vec3 vNormal;
out vec3 vColor;
void main() {
    vec3 p = vec3(model * vec4(posicion, 1.0));
    // Pasto fijo: sigue la transformacion manual del piso.
    vPos = p;
    vNormal = mat3(transpose(inverse(model))) * normal;
    vColor = color;
    gl_Position = projection * view * vec4(p, 1.0);
}
)GLSL";

const char* paisajeFS = R"GLSL(
#version 330 core
in vec3 vPos;
in vec3 vNormal;
in vec3 vColor;
out vec4 fragColor;
uniform mat4 view;
uniform float unidad;
uniform int esPasto;
uniform vec2 centroPerro;
uniform vec2 radioSombra;
float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1,311.7))) * 43758.5453); }
void main() {
    vec3 luz = normalize(vec3(-0.5, 1.0, 0.45));
    float difusa = max(dot(normalize(vNormal), luz), 0.0);
    vec3 c = vColor * (0.62 + 0.48 * difusa);
    if (esPasto == 0) {
        // Moteado fino que evita que la planicie parezca un plastico liso.
        float detalle = hash(floor(vPos.xz / unidad * 65.0));
        c *= 0.93 + detalle * 0.14;
        // Sombra de contacto aproximada, pintada bajo el perro (no shadow map).
        vec2 d = (vPos.xz - centroPerro) / radioSombra;
        c *= 1.0 - 0.29 * exp(-dot(d,d) * 1.8);
    }
    float distancia = length((view * vec4(vPos,1.0)).xyz) / unidad;
    float niebla = smoothstep(12.0, 32.0, distancia) * 0.42;
    c = mix(c, vec3(0.50, 0.78, 0.97), niebla);
    fragColor = vec4(c, 1.0);
}
)GLSL";

GLuint Compilar(GLenum tipo, const char* codigo)
{
    GLuint shader = glCreateShader(tipo);
    glShaderSource(shader, 1, &codigo, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[4096] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Error shader paisaje: ") + log);
    }
    return shader;
}

// Fondo de pantalla: nube difusa calculada por pixel, sin malla ni textura.
// El rayo de vision conserva la direccion de la nube al girar la camara.
const char* cieloVS = R"GLSL(
#version 330 core
out vec2 ndc;
void main()
{
    vec2 p;
    if (gl_VertexID == 0) p = vec2(-1.0, -1.0);
    else if (gl_VertexID == 1) p = vec2(3.0, -1.0);
    else p = vec2(-1.0, 3.0);
    ndc = p;
    gl_Position = vec4(p, 0.0, 1.0);
}
)GLSL";

const char* cieloFS = R"GLSL(
#version 330 core
in vec2 ndc;
out vec4 fragColor;
uniform mat4 inversaProyeccion;
uniform mat3 orientacionCamara;
uniform float cantidadNubes;

float hash2(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float ruido(vec2 p)
{
    vec2 celda = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash2(celda);
    float b = hash2(celda + vec2(1.0, 0.0));
    float c = hash2(celda + vec2(0.0, 1.0));
    float d = hash2(celda + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float detalle(vec2 p)
{
    float suma = 0.0;
    float peso = 0.5;
    for (int i = 0; i < 4; ++i)
    {
        suma += peso * ruido(p);
        p = p * 2.02 + vec2(11.7, 7.3);
        peso *= 0.5;
    }
    return suma / 0.9375;
}

void main()
{
    vec4 ojo = inversaProyeccion * vec4(ndc, 1.0, 1.0);
    vec3 dir = normalize(orientacionCamara * (ojo.xyz / ojo.w));
    float altura = asin(clamp(dir.y, -1.0, 1.0));
    float longitud = atan(dir.x, -dir.z);

    // Azul mas claro cerca del horizonte, azul vivo en la parte alta.
    float t = smoothstep(-0.13, 1.04, altura);
    vec3 cielo = mix(vec3(0.68, 0.89, 1.00),
                    vec3(0.17, 0.54, 0.88), t);
    cielo += vec3(0.015, 0.025, 0.025) * (1.0 - t);

    // Dos escalas y deformacion lenta: masas amplias con bordes vaporosos.
    // No depende del tiempo; las nubes no se mueven.
    vec2 p = vec2(longitud * 4.2, altura * 7.4);
    vec2 deformacion = vec2(detalle(p * 0.43 + vec2(4.1, 7.2)),
                            detalle(p * 0.43 + vec2(18.5, 1.6)));
    vec2 q = p + (deformacion - 0.5) * 1.65;
    float masa = detalle(q * vec2(1.05, 0.68));
    float borde = ruido(q * vec2(3.6, 1.9) + vec2(3.4, 8.6));
    float densidad = masa * 0.82 + borde * 0.18;
    float nubes = smoothstep(0.47, 0.69,
        densidad + (cantidadNubes - 0.55) * 0.16);

    // Nubes mas suaves en el horizonte y zonas limpias de cielo arriba.
    float velo = smoothstep(0.47, 0.71,
        detalle(q * vec2(0.62, 0.42) + vec2(10.0, 2.0))) * 0.28;
    float mezcla = clamp(max(nubes * 0.85, velo), 0.0, 0.88);
    mezcla *= smoothstep(-0.18, 0.04, altura);

    // Blanco calido donde da la luz y cian suave en la parte inferior.
    float luz = clamp(0.68 + 0.28 * borde + altura * 0.10, 0.0, 1.0);
    vec3 colorNube = mix(vec3(0.68, 0.86, 0.94),
                         vec3(1.0, 0.99, 0.97), luz);
    fragColor = vec4(mix(cielo, colorNube, mezcla), 1.0);
}
)GLSL";

GLuint CrearProgramaCielo()
{
    GLuint vs = Compilar(GL_VERTEX_SHADER, cieloVS);
    GLuint fs = Compilar(GL_FRAGMENT_SHADER, cieloFS);
    GLuint programa = glCreateProgram();
    glAttachShader(programa, vs);
    glAttachShader(programa, fs);
    glLinkProgram(programa);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = GL_FALSE;
    glGetProgramiv(programa, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[4096] = {};
        glGetProgramInfoLog(programa, sizeof(log), nullptr, log);
        glDeleteProgram(programa);
        throw std::runtime_error(std::string("Error enlace cielo: ") + log);
    }
    return programa;
}

GLuint CrearProgramaPaisaje()
{
    GLuint vs = Compilar(GL_VERTEX_SHADER, paisajeVS);
    GLuint fs = Compilar(GL_FRAGMENT_SHADER, paisajeFS);
    GLuint programa = glCreateProgram();
    glAttachShader(programa, vs); glAttachShader(programa, fs);
    glLinkProgram(programa);
    glDeleteShader(vs); glDeleteShader(fs);
    GLint ok = GL_FALSE;
    glGetProgramiv(programa, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[4096] = {};
        glGetProgramInfoLog(programa, sizeof(log), nullptr, log);
        glDeleteProgram(programa);
        throw std::runtime_error(std::string("Error enlace paisaje: ") + log);
    }
    return programa;
}

void Matriz(GLuint programa, const char* nombre, const glm::mat4& matriz)
{
    glUniformMatrix4fv(glGetUniformLocation(programa, nombre), 1, GL_FALSE, glm::value_ptr(matriz));
}

int main()
{
    if (!glfwInit()) return EXIT_FAILURE;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica #6 Bello Zaragoza Demian", nullptr, nullptr);
    if (!window) { glfwTerminate(); return EXIT_FAILURE; }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { glfwDestroyWindow(window); glfwTerminate(); return EXIT_FAILURE; }
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE); // Las hojas deben verse por ambos lados.

    int resultado = EXIT_SUCCESS;
    try
    {
        MedirPerro("Models/RedDog.obj");
        Shader shaderPerro("Shader/modelLoading.vs", "Shader/modelLoading.frag");
        Model dog((char*)"Models/RedDog.obj");

        Model llama((char*)"Models/Llama/Llama.obj");

        glm::vec3 cofreMin, cofreMax;
        MedirOBJ("Models/Chest/coffre_autosave_1.obj", cofreMin, cofreMax);
        Model cofre((char*)"Models/Chest/coffre_autosave_1.obj");

        // BIGPOT: modelo nuevo con su MTL y textura Image_BIGPOT.jpg.
        glm::vec3 bigPotMin, bigPotMax;
        MedirOBJ("Models/BigPot/Bigpot.obj", bigPotMin, bigPotMax);
        Model bigPot((char*)"Models/BigPot/Bigpot.obj");

        float alturaPerro = perroMax.y - perroMin.y;
        float alturaCofre = cofreMax.y - cofreMin.y;
        float alturaBigPot = bigPotMax.y - bigPotMin.y;

        // CAJA DE MUNICIONES
        glm::vec3 municionMin, municionMax;
        MedirOBJ(
            "Models/Ammo/ammo_box_-_fortnite.obj",
            municionMin,
            municionMax
        );
        Model municion((char*)"Models/Ammo/ammo_box_-_fortnite.obj");

        // ARBOL TEXTURIZADO.
        // Ahora EmeraldTree.mtl referencia Image_0.jpg, asi que se carga con Model
        // para respetar las UV y la textura exportada desde Blender.
        glm::vec3 arbolMin, arbolMax;
        MedirOBJ("Models/EmeraldTree/EmeraldTree.obj", arbolMin, arbolMax);
        Model arbol((char*)"Models/EmeraldTree/EmeraldTree.obj");

        // Ambos arbustos usan el mismo OBJ y conservan posiciones independientes.
        glm::vec3 arbustoMin, arbustoMax;
        MedirOBJ("Models/Bush/Fortnite_Bush.obj", arbustoMin, arbustoMax);
        Model arbusto((char*)"Models/Bush/Fortnite_Bush.obj");

        // Cuatro tipos de roca nuevos.
        // Roca1 no tiene textura; las cuatro se dibujan con el shader de color
        // para que no dependan de map_Kd ni salgan negras.
        glm::vec3 rocaMin[4], rocaMax[4];

        MedirOBJ("Models/Rocks/Roca1.obj", rocaMin[0], rocaMax[0]);
        MedirOBJ("Models/Rocks/roca2 - copia.obj", rocaMin[1], rocaMax[1]);
        MedirOBJ("Models/Rocks/roca3 - copia.obj", rocaMin[2], rocaMax[2]);
        MedirOBJ("Models/Rocks/roca4 - copia.obj", rocaMin[3], rocaMax[3]);

        // =============================================================
        // ACOMODA TODOS LOS ELEMENTOS AQUI
        // =============================================================
        // Cada vector es (X, Y, Z).
        // POSICION: coordenadas absolutas del pivote, en unidades OpenGL.
        // ROTACION: grados. ESCALA: factores directos del modelo.
        // Puedes reemplazar CUALQUIER expresion por un numero como 1.5f.
        // Las expresiones iniciales solo conservan el acomodo que ya tenias.
        // No hay ajuste automatico de altura: TU decides el valor de Y.
        // Guarda, recompila y ejecuta para aplicar los cambios.

        // ---------------- PERRITO ----------------
        glm::vec3 perroPosicion(centroX, pisoY, centroZ);
        glm::vec3 perroRotacion(0.0f, 30.0f, 0.0f);
        glm::vec3 perroEscala(1.0f, 1.0f, 1.0f);

        // ---------------- LLAMA ------------------
        glm::vec3 llamaPosicion(centroX - 1.5f, pisoY, centroZ - 1.95f * unidad);
        glm::vec3 llamaRotacion(0.0f, 45.0f, 0.0f);
        glm::vec3 llamaEscala(alturaPerro * 2.25f / 1.206798f);
        // Por ejemplo: glm::vec3 llamaEscala(0.5f, 0.5f, 0.5f);

        // ---------------- COFRE ------------------
        glm::vec3 cofrePosicion(centroX + 1.1f, pisoY, centroZ - 0.950f * unidad);
        glm::vec3 cofreRotacion(0.0f, -45.0f, 0.0f);
        glm::vec3 cofreEscala(alturaPerro * 0.75f / alturaCofre);
        // Para subirlo: cambia SOLO el segundo valor de cofrePosicion.
        // Para girarlo: cambia cofreRotacion, por ejemplo (0, 45, 0).

        // ---------------- BIGPOT -----------------
        // Se coloca delante del cofre (hacia la camara = Z mas positivo).
        // Cambia estos tres bloques si quieres moverlo, girarlo o escalarlo.
        glm::vec3 bigPotPosicion(
            cofrePosicion.x - 0.15f * unidad,
            pisoY,
            cofrePosicion.z + 0.95f * unidad
        );
        glm::vec3 bigPotRotacion(0.0f, -25.0f, 0.0f);

        // Un poco menor que el cofre para que no lo tape por completo.
        float escalaBaseBigPot = (0.21f, 0.21f, 0.21f);
        glm::vec3 bigPotEscala(escalaBaseBigPot);

        // ---------------- CAJA DE MUNICIONES ----
        // Coordenadas editables e independientes: X derecha, Y altura, Z fondo.
        // Si tu cofre ya esta en otra posicion, ajusta estos tres valores.
        glm::vec3 municionPosicion(cofrePosicion.x + 0.75f * unidad,
            pisoY, cofrePosicion.z + 0.15f * unidad);
        glm::vec3 municionRotacion(0.0f, 0.0f, 0.0f);
        glm::vec3 municionEscala(
            (alturaCofre * cofreEscala.y * 0.7f) / (municionMax.y - municionMin.y));

        // ---------------- ARBOLES ----------------
        // Ajusta cada arbol por separado: (X, Y, Z), giro en grados, escala.
        float escalaBaseArbol = alturaPerro * 5.5f / (arbolMax.y - arbolMin.y);
        glm::vec3 arbolIzquierdoPosicion(centroX - 2.8f * unidad,
            pisoY + 1.8f, centroZ - 14.75f * unidad);
        glm::vec3 arbolIzquierdoRotacion(0.0f, 90.0f, 0.0f);
        glm::vec3 arbolIzquierdoEscala(escalaBaseArbol * 1.2);

        glm::vec3 arbolDerechoPosicion(centroX + 2.8f * unidad,
            pisoY + 0.1f, centroZ - 9.5f * unidad);
        glm::vec3 arbolDerechoRotacion(0.0f, -35.0f, 0.0f);
        glm::vec3 arbolDerechoEscala(escalaBaseArbol * 1.50f);

        // Dos arboles adicionales al fondo para llenar mejor el horizonte.
        glm::vec3 arbolFondoIzqPosicion(
            centroX + 1.3f * unidad,
            pisoY +1.8f,
            centroZ - 16.6f * unidad
        );
        glm::vec3 arbolFondoIzqRotacion(0.0f, 28.0f, 0.0f);
        glm::vec3 arbolFondoIzqEscala(escalaBaseArbol * 0.95f);

        glm::vec3 arbolFondoDerPosicion(
            centroX + 6.6f * unidad,
            pisoY + 0.50f,
            centroZ - 17.2f * unidad
        );
        glm::vec3 arbolFondoDerRotacion(0.0f, -70.0f, 0.0f);
        glm::vec3 arbolFondoDerEscala(escalaBaseArbol * 1.05f);

        // ---------------- ARBUSTOS ---------------
        // Centro atras del perro; el otro delante, a su izquierda.
        // Cambia X, Y y Z de cada uno libremente, igual que los demas modelos.
        float escalaBaseArbusto = alturaPerro * 2.5f / (arbustoMax.y - arbustoMin.y);
        glm::vec3 arbustoAtrasPosicion(centroX, pisoY,
            centroZ - 5.0f * unidad);
        glm::vec3 arbustoAtrasRotacion(0.0f, 25.0f, 0.0f);
        glm::vec3 arbustoAtrasEscala(escalaBaseArbusto * 0.60f);

        glm::vec3 arbustoDelantePosicion(centroX - 1.8f * unidad, pisoY,
            centroZ + 0.5f * unidad);
        glm::vec3 arbustoDelanteRotacion(0.0f, -30.0f, 0.0f);
        glm::vec3 arbustoDelanteEscala(escalaBaseArbusto * 0.45f);

        // ---------------- ROCAS -----------------
        // 12 rocas distribuidas por el entorno usando 4 modelos diferentes.
        // Puedes mover cualquiera cambiando su glm::vec3 de posicion.
        const int CANTIDAD_ROCAS = 13;
        const int TIPO_ROCA[CANTIDAD_ROCAS] = {
            0, 1, 2, 3,
            1, 3, 0, 2,
            3, 0, 2, 1,
            0
        };

        glm::vec3 rocaPosicion[CANTIDAD_ROCAS] = {
            // Fondo / colina
            glm::vec3(centroX - 5.8f * unidad,
                AlturaTerreno(centroX - 5.8f * unidad, centroZ - 8.2f * unidad + 4.5f) - 0.05f * unidad,
                centroZ - 8.2f * unidad),

            glm::vec3(centroX + 5.2f * unidad,
                AlturaTerreno(centroX + 5.2f * unidad, centroZ - 9.0f * unidad + 4.5f) - 0.05f * unidad,
                centroZ - 9.0f * unidad),

            glm::vec3(centroX - 2.2f * unidad,
                AlturaTerreno(centroX - 2.2f * unidad, centroZ - 11.4f * unidad + 4.5f) - 0.05f * unidad,
                centroZ - 11.4f * unidad),

            glm::vec3(centroX + 2.9f * unidad,
                AlturaTerreno(centroX + 2.9f * unidad, centroZ - 12.6f * unidad + 4.5f) - 0.05f * unidad,
                centroZ - 12.6f * unidad),

                // Zona media
                glm::vec3(centroX - 4.3f * unidad,
                    AlturaTerreno(centroX - 4.3f * unidad, centroZ - 4.8f * unidad + 4.5f) - 0.04f * unidad,
                    centroZ - 4.8f * unidad),

                glm::vec3(centroX + 4.6f * unidad,
                    AlturaTerreno(centroX + 4.6f * unidad, centroZ - 5.7f * unidad + 4.5f) - 0.04f * unidad,
                    centroZ - 5.7f * unidad),

                glm::vec3(centroX + 0.9f * unidad,
                    AlturaTerreno(centroX + 0.9f * unidad, centroZ - 7.0f * unidad + 4.5f) - 0.04f * unidad,
                    centroZ - 7.0f * unidad),

                glm::vec3(centroX - 1.3f * unidad,
                    AlturaTerreno(centroX - 1.3f * unidad, centroZ - 3.2f * unidad + 4.5f) - 0.04f * unidad,
                    centroZ - 3.2f * unidad),

                    // Primer plano / lados
                    glm::vec3(centroX - 4.8f * unidad, pisoY - 0.08f * unidad,
                        centroZ + 1.2f * unidad),

                    glm::vec3(centroX + 4.5f * unidad, pisoY - 0.08f * unidad,
                        centroZ + 1.8f * unidad),

                    glm::vec3(centroX - 2.6f * unidad, pisoY - 0.07f * unidad,
                        centroZ + 4.0f * unidad),

                    glm::vec3(centroX + 2.4f * unidad, pisoY - 0.07f * unidad,
                        centroZ + 4.7f * unidad),

                        // Piedra nueva justo delante del perrito.
                        glm::vec3(
                            perroPosicion.x - 0.85f * unidad,
                            pisoY - 0.15f * unidad,
                            perroPosicion.z + 1.15f * unidad
                        )
        };

        glm::vec3 rocaRotacion[CANTIDAD_ROCAS] = {
            glm::vec3(0,  18,   4),
            glm::vec3(0,  72,  -5),
            glm::vec3(0, -38,   3),
            glm::vec3(0, 138,  -7),
            glm::vec3(0, 105,   5),
            glm::vec3(0, -62,  -4),
            glm::vec3(0,  34,   2),
            glm::vec3(0, 164,  -6),
            glm::vec3(0, -84,   8),
            glm::vec3(0,  51,  -9),
            glm::vec3(0, 121,   5),
            glm::vec3(0, -21,  -4),
            glm::vec3(0,  42,   2)
        };

        // Altura aproximada de cada roca respecto al perro.
        float alturasRoca[CANTIDAD_ROCAS] = {
            0.90f, 0.48f, 0.66f, 0.55f,
            0.34f, 0.72f, 0.30f, 0.38f,
            1.05f, 0.82f, 0.43f, 0.60f,
            0.52f
        };

        glm::vec3 rocaEscala[CANTIDAD_ROCAS];

        for (int i = 0; i < CANTIDAD_ROCAS; ++i)
        {
            int tipo = TIPO_ROCA[i];

            float escala = alturaPerro * alturasRoca[i] /
                (rocaMax[tipo].y - rocaMin[tipo].y);

            // Pequeñas variaciones para que incluso el mismo modelo parezca distinto.
            float anchoX = (i % 3 == 0) ? 1.18f : ((i % 3 == 1) ? 0.92f : 1.05f);
            float altoY = (i % 4 == 0) ? 0.88f : 1.0f;
            float fondoZ = (i % 2 == 0) ? 1.08f : 0.90f;

            rocaEscala[i] = glm::vec3(
                escala * anchoX,
                escala * altoY,
                escala * fondoZ
            );
        }

        // ---------------- PISO Y PASTO -----------
        // Se transforman juntos porque el pasto nace de la misma superficie.
        glm::vec3 pisoPosicion(centroX, pisoY, centroZ - 4.5f);
        glm::vec3 pisoRotacion(0.0f, 0.0f, 0.0f);
        glm::vec3 pisoEscala(1.0f, 1.0f, 1.0f);
        // La colina es parte del piso. Su forma se edita en COLINA_POSICION,
        // COLINA_ESCALA y COLINA_GIRO_Y, al inicio de este archivo.

        // ---------------- CAMARA INICIAL ---------
        glm::vec3 camaraPosicion(centroX, pisoY + unidad * 0.65f,
            perroMax.z + unidad * 4.0f); // Se aleja para mostrar el cofre y la caja.
        // =============================================================
        // FIN DE LOS AJUSTES. Debajo se aplican los valores tal cual.
        // =============================================================

        glm::vec3 pivotePerro(centroX, perroMin.y, centroZ);
        glm::vec3 pivoteLlama(0.0f, 0.000625f, -0.0535435f);
        glm::vec3 pivoteCofre((cofreMin.x + cofreMax.x) * 0.5f, cofreMin.y,
            (cofreMin.z + cofreMax.z) * 0.5f);

        glm::vec3 pivoteBigPot(
            (bigPotMin.x + bigPotMax.x) * 0.5f,
            bigPotMin.y,
            (bigPotMin.z + bigPotMax.z) * 0.5f
        );

        glm::mat4 modelDog = TransformarModelo(perroPosicion, perroRotacion, perroEscala, pivotePerro);
        glm::mat4 modelLlama = TransformarModelo(llamaPosicion, llamaRotacion, llamaEscala, pivoteLlama);
        glm::mat4 modelCofre = TransformarModelo(cofrePosicion, cofreRotacion, cofreEscala, pivoteCofre);
        glm::mat4 modelBigPot = TransformarModelo(
            bigPotPosicion, bigPotRotacion, bigPotEscala, pivoteBigPot);

        // Disco dorado apenas por encima del piso del BigPot.
        glm::mat4 modelBrilloBigPot(1.0f);
        modelBrilloBigPot = glm::translate(
            modelBrilloBigPot,
            glm::vec3(bigPotPosicion.x, bigPotPosicion.y + 0.012f * unidad, bigPotPosicion.z)
        );
        modelBrilloBigPot = glm::scale(
            modelBrilloBigPot,
            glm::vec3(unidad * 0.72f, 1.0f, unidad * 0.55f)
        );

        // Resplandor dorado del cofre: más amplio e intenso, como en Fortnite.
        glm::mat4 modelBrilloCofre(1.0f);
        modelBrilloCofre = glm::translate(
            modelBrilloCofre,
            glm::vec3(cofrePosicion.x, cofrePosicion.y + 0.014f * unidad, cofrePosicion.z)
        );
        modelBrilloCofre = glm::scale(
            modelBrilloCofre,
            glm::vec3(unidad * 1.10f, 1.0f, unidad * 0.82f)
        );

        glm::vec3 pivoteMunicion((municionMin.x + municionMax.x) * 0.5f,
            municionMin.y, (municionMin.z + municionMax.z) * 0.5f);
        glm::mat4 modelMunicion = TransformarModelo(
            municionPosicion, municionRotacion, municionEscala, pivoteMunicion);
        glm::vec3 pivoteArbol((arbolMin.x + arbolMax.x) * 0.5f, arbolMin.y,
            (arbolMin.z + arbolMax.z) * 0.5f);
        glm::mat4 modelArbolIzquierdo = TransformarModelo(
            arbolIzquierdoPosicion, arbolIzquierdoRotacion, arbolIzquierdoEscala, pivoteArbol);
        glm::mat4 modelArbolDerecho = TransformarModelo(
            arbolDerechoPosicion, arbolDerechoRotacion, arbolDerechoEscala, pivoteArbol);
        glm::mat4 modelArbolFondoIzq = TransformarModelo(
            arbolFondoIzqPosicion, arbolFondoIzqRotacion, arbolFondoIzqEscala, pivoteArbol);
        glm::mat4 modelArbolFondoDer = TransformarModelo(
            arbolFondoDerPosicion, arbolFondoDerRotacion, arbolFondoDerEscala, pivoteArbol);

        glm::vec3 pivoteArbusto((arbustoMin.x + arbustoMax.x) * 0.5f,
            arbustoMin.y, (arbustoMin.z + arbustoMax.z) * 0.5f);
        glm::mat4 modelArbustoAtras = TransformarModelo(
            arbustoAtrasPosicion, arbustoAtrasRotacion, arbustoAtrasEscala, pivoteArbusto);
        glm::mat4 modelArbustoDelante = TransformarModelo(
            arbustoDelantePosicion, arbustoDelanteRotacion, arbustoDelanteEscala, pivoteArbusto);
        glm::mat4 modelRoca[CANTIDAD_ROCAS];
        for (int i = 0; i < CANTIDAD_ROCAS; ++i)
        {
            int tipo = TIPO_ROCA[i];
            glm::vec3 pivote((rocaMin[tipo].x + rocaMax[tipo].x) * 0.5f,
                rocaMin[tipo].y, (rocaMin[tipo].z + rocaMax[tipo].z) * 0.5f);
            modelRoca[i] = TransformarModelo(rocaPosicion[i], rocaRotacion[i], rocaEscala[i], pivote);
        }
        matrizPaisaje = TransformarModelo(pisoPosicion, pisoRotacion, pisoEscala,
            glm::vec3(centroX, pisoY, centroZ));

        for (int i = 0; i < CANTIDAD_ROCAS; ++i)
            LimitesTransformados(modelRoca[i], rocaMin[TIPO_ROCA[i]], rocaMax[TIPO_ROCA[i]],
                rocaMundoMin[i], rocaMundoMax[i]);
        LimitesTransformados(modelDog, perroMin, perroMax, perroMundoMin, perroMundoMax);
        LimitesTransformados(modelLlama,
            glm::vec3(-0.211756f, 0.000625f, -0.480119f),
            glm::vec3(0.211756f, 1.207423f, 0.373032f), llamaMundoMin, llamaMundoMax);
        LimitesTransformados(modelCofre, cofreMin, cofreMax, cofreMundoMin, cofreMundoMax);
        LimitesTransformados(modelBigPot, bigPotMin, bigPotMax, bigPotMundoMin, bigPotMundoMax);
        LimitesTransformados(modelMunicion, municionMin, municionMax,
            municionMundoMin, municionMundoMax);
        LimitesTransformados(modelArbustoAtras, arbustoMin, arbustoMax,
            arbustoAtrasMundoMin, arbustoAtrasMundoMax);
        LimitesTransformados(modelArbustoDelante, arbustoMin, arbustoMax,
            arbustoDelanteMundoMin, arbustoDelanteMundoMax);
        glm::vec3 centroSombra = (perroMundoMin + perroMundoMax) * 0.5f;

        GLuint paisaje = CrearProgramaPaisaje();
        GLuint brillo = CrearProgramaBrillo();
        GLuint cielo = CrearProgramaCielo();
        GLuint pantallaVAO = 0;
        glGenVertexArrays(1, &pantallaVAO);

        MallaPaisaje terreno, pasto, brilloBigPot;
        MallaPaisaje rocaMalla[4];

        CrearTerreno(terreno);
        CrearPasto(pasto);
        CrearDiscoBrillo(brilloBigPot);

        // Cargar los cuatro modelos nuevos con colores ligeramente distintos.
        // Esto evita que salgan negros aunque sus MTL no tengan textura map_Kd.
        CrearRocaColorDesdeOBJ(
            rocaMalla[0],
            "Models/Rocks/Roca1.obj",
            glm::vec3(0.68f, 0.70f, 0.72f)
        );

        CrearRocaColorDesdeOBJ(
            rocaMalla[1],
            "Models/Rocks/roca2 - copia.obj",
            glm::vec3(0.58f, 0.61f, 0.64f)
        );

        CrearRocaColorDesdeOBJ(
            rocaMalla[2],
            "Models/Rocks/roca3 - copia.obj",
            glm::vec3(0.72f, 0.70f, 0.66f)
        );

        CrearRocaColorDesdeOBJ(
            rocaMalla[3],
            "Models/Rocks/roca4 - copia.obj",
            glm::vec3(0.55f, 0.59f, 0.62f)
        );
        // Encuadre inicial: ve al perro de frente y las colinas detras de el.
        camera = Camera(camaraPosicion);
        lastFrame = static_cast<float>(glfwGetTime());
        std::cout << "WASD/flechas: mover | Mouse: mirar | G: pasto | Esc: salir\n";

        while (!glfwWindowShouldClose(window))
        {
            float tiempo = static_cast<float>(glfwGetTime());
            deltaTime = glm::min(tiempo - lastFrame, 0.05f);
            lastFrame = tiempo;
            glfwPollEvents();
            DoMovement();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Perspectiva en radianes; no depende de las unidades de GetZoom().
            glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT,
                unidad * 0.015f, unidad * 80.0f);
            glm::mat4 view = camera.GetViewMatrix();

            // Primero el cielo, luego el suelo y los modelos frente a el.
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glUseProgram(cielo);
            Matriz(cielo, "inversaProyeccion", glm::inverse(projection));
            glm::mat3 orientacionCielo(glm::inverse(view));
            glUniformMatrix3fv(glGetUniformLocation(cielo, "orientacionCamara"),
                1, GL_FALSE, glm::value_ptr(orientacionCielo));
            glUniform1f(glGetUniformLocation(cielo, "cantidadNubes"), CANTIDAD_NUBES_CIELO);
            glBindVertexArray(pantallaVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);

            glUseProgram(paisaje);
            Matriz(paisaje, "projection", projection);
            Matriz(paisaje, "view", view);
            Matriz(paisaje, "model", matrizPaisaje);
            glUniform1f(glGetUniformLocation(paisaje, "unidad"), unidad);
            glUniform2f(glGetUniformLocation(paisaje, "centroPerro"), centroSombra.x, centroSombra.z);
            glUniform2f(glGetUniformLocation(paisaje, "radioSombra"),
                glm::max((perroMundoMax.x - perroMundoMin.x) * 0.6f, unidad * 0.15f),
                glm::max((perroMundoMax.z - perroMundoMin.z) * 0.6f, unidad * 0.15f));
            glUniform1i(glGetUniformLocation(paisaje, "esPasto"), 0);
            terreno.Dibujar();
            if (mostrarPasto)
            {
                glUniform1i(glGetUniformLocation(paisaje, "esPasto"), 1);
                pasto.Dibujar();
            }

            // Cambiar al shader de modelos texturizados.
            shaderPerro.Use();
            Matriz(shaderPerro.Program, "projection", projection);
            Matriz(shaderPerro.Program, "view", view);

            // Cuatro instancias del arbol usando la textura real de EmeraldTree.mtl.
            Matriz(shaderPerro.Program, "model", modelArbolIzquierdo);
            arbol.Draw(shaderPerro);

            Matriz(shaderPerro.Program, "model", modelArbolDerecho);
            arbol.Draw(shaderPerro);

            Matriz(shaderPerro.Program, "model", modelArbolFondoIzq);
            arbol.Draw(shaderPerro);

            Matriz(shaderPerro.Program, "model", modelArbolFondoDer);
            arbol.Draw(shaderPerro);
            // Dos instancias del arbusto, con transformaciones independientes.
            Matriz(shaderPerro.Program, "model", modelArbustoAtras);
            arbusto.Draw(shaderPerro);
            Matriz(shaderPerro.Program, "model", modelArbustoDelante);
            arbusto.Draw(shaderPerro);

            // Dibujar las 12 rocas usando los 4 modelos nuevos.
            glUseProgram(paisaje);
            Matriz(paisaje, "projection", projection);
            Matriz(paisaje, "view", view);
            glUniform1f(glGetUniformLocation(paisaje, "unidad"), unidad);
            glUniform1i(glGetUniformLocation(paisaje, "esPasto"), 1);

            for (int i = 0; i < CANTIDAD_ROCAS; ++i)
            {
                Matriz(paisaje, "model", modelRoca[i]);
                rocaMalla[TIPO_ROCA[i]].Dibujar();
            }

            // Regresar al shader de los modelos texturizados.
            shaderPerro.Use();
            Matriz(shaderPerro.Program, "projection", projection);
            Matriz(shaderPerro.Program, "view", view);

            // Aplicar la posicion, rotacion y escala elegidas para el perro.
            Matriz(shaderPerro.Program, "model", modelDog);
            dog.Draw(shaderPerro);

            // Dibujar la llama con el mismo shader de modelos texturizados.
            Matriz(shaderPerro.Program, "model", modelLlama);
            llama.Draw(shaderPerro);

            // ---------------------------------------------------------
            // BRILLOS DORADOS: cofre + BigPot.
            // El del cofre es más amplio para imitar el resplandor de Fortnite.
            // ---------------------------------------------------------
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);

            glUseProgram(brillo);
            Matriz(brillo, "projection", projection);
            Matriz(brillo, "view", view);

            Matriz(brillo, "model", modelBrilloCofre);
            brilloBigPot.Dibujar();

            Matriz(brillo, "model", modelBrilloBigPot);
            brilloBigPot.Dibujar();

            glDepthMask(GL_TRUE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_BLEND);

            // Volver al shader de modelos y dibujar BigPot delante del cofre.
            shaderPerro.Use();
            Matriz(shaderPerro.Program, "projection", projection);
            Matriz(shaderPerro.Program, "view", view);
            Matriz(shaderPerro.Program, "model", modelBigPot);
            bigPot.Draw(shaderPerro);

            // COFRE: transformacion propia, independiente de perro y llama.
            Matriz(shaderPerro.Program, "model", modelCofre);
            cofre.Draw(shaderPerro);

            // Dibujar la caja de municiones.
            Matriz(shaderPerro.Program, "model", modelMunicion);
            municion.Draw(shaderPerro);
            glfwSwapBuffers(window);
        }
        terreno.Liberar();
        pasto.Liberar();
        brilloBigPot.Liberar();
        for (int i = 0; i < 4; ++i)
            rocaMalla[i].Liberar();

        glDeleteProgram(brillo);
        glDeleteProgram(paisaje);
        glDeleteProgram(cielo);
        glDeleteVertexArrays(1, &pantallaVAO);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        resultado = EXIT_FAILURE;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return resultado;
}

void DoMovement()
{
    // Velocidad proporcional al modelo; camara libre, sin colisiones.
    float paso = deltaTime * unidad * 0.5f;
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, paso);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, paso);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, paso);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, paso);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_G && action == GLFW_PRESS) mostrarPasto = !mostrarPasto;
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS) keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xPos); lastY = static_cast<float>(yPos);
        firstMouse = false;
    }
    float xOffset = static_cast<float>(xPos) - lastX;
    float yOffset = lastY - static_cast<float>(yPos);
    lastX = static_cast<float>(xPos); lastY = static_cast<float>(yPos);
    camera.ProcessMouseMovement(xOffset, yOffset);
}
