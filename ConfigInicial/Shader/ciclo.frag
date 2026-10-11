#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
out vec4 color;
uniform vec3 viewPos;
// Cada astro usa los atributos de light que enviaba el main original.
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light sun;
uniform Light moon;
uniform vec3 objectColor;
uniform bool useTexture;
uniform bool isGround;
uniform bool flipDogV;
uniform float alphaCutoff; // 0 en perro/piso; 0.5 para recortar hojas del arbol.
uniform sampler2D albedoMap;

float pixelNoise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Textura procedural de 16x16 por bloque de 2 unidades, sin tablero alternado.
vec3 MinecraftGround(vec3 N) {
    bool top = N.y > 0.5;
    vec2 uv = top ? FragPos.xz / 2.0
                 : vec2(abs(N.x) > 0.5 ? FragPos.z : FragPos.x, FragPos.y) / 2.0;
    vec2 pixel = mod(floor(uv * 16.0), 16.0);
    float fine = pixelNoise(pixel);
    float grassVariation = pixelNoise(floor(pixel / 2.0) + vec2(41.0, 17.0));
    // Pequeñas manchas cuadradas con varios verdes, como pasto de bloques.
    vec3 grass = mix(vec3(0.24,0.46,0.10), vec3(0.44,0.67,0.19), grassVariation);
    grass *= mix(0.85, 1.12, floor(fine * 4.0) / 3.0);
    vec3 dirt = mix(vec3(0.30,0.19,0.11), vec3(0.52,0.36,0.22), grassVariation);
    dirt *= mix(0.80,1.16,floor(fine*4.0)/3.0);
    if (fine > 0.93) dirt = vec3(0.47,0.43,0.34); // Piedritas en la tierra.
    // Franja irregular de pasto en la parte alta de los laterales del bloque.
    float fringe = 0.16 + 0.18 * pixelNoise(vec2(pixel.x, 83.0));
    bool grassSide = !top && N.y > -0.5 && fract(uv.y) > 1.0-fringe;
    vec3 texel = (top || grassSide) ? grass : dirt;
    vec3 average = (top || grassSide) ? vec3(0.34,0.56,0.145) : vec3(0.41,0.28,0.17);
    // Suaviza solo los pixeles subpixel a distancia para evitar parpadeo.
    vec2 footprint = fwidth(uv * 16.0);
    float fade = smoothstep(0.65,1.5,max(footprint.x,footprint.y));
    return mix(texel,average,fade);
}

vec3 CalculateLight(Light light, vec3 base, vec3 N, vec3 V)
{
    // Luz PUNTUAL: direccion desde ESTE fragmento hasta el astro visible.
    vec3 offset = light.position - FragPos;
    vec3 L = offset / max(length(offset), 0.0001);
    float diff = max(dot(N, L), 0.0);
    vec3 reflected = reflect(-L, N);
    float spec = diff > 0.0 ? pow(max(dot(V, reflected), 0.0), 32.0) : 0.0;
    vec3 ambient = light.ambient * base;
    vec3 diffuse = light.diffuse * diff * base;
    // Acabado mate para el perro y el pasto, sin reflejos de plastico intensos.
    vec3 specular = light.specular * spec * (isGround ? 0.015 : 0.045);
    return ambient + diffuse + specular;
}

void main()
{
    vec3 base = objectColor;
    if (useTexture) {
        vec2 uv = vec2(TexCoords.x, flipDogV ? 1.0-TexCoords.y : TexCoords.y);
        vec4 texel = texture(albedoMap, uv);
        if (texel.a < alphaCutoff) discard;
        base *= texel.rgb;
    }
    vec3 N = normalize(Normal);
    if (isGround) base *= MinecraftGround(N);
    vec3 V = normalize(viewPos - FragPos);
    color = vec4(CalculateLight(sun, base, N, V)
               + CalculateLight(moon, base, N, V), 1.0);
}
