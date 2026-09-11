#version 330 core

in vec3 localPos;

out vec4 color;

uniform vec3 objectColor;
uniform vec3 objectScale;

float random(vec2 p)
{
    return fract(
        sin(dot(p, vec2(12.9898, 78.233))) *
        43758.5453
    );
}

void main()
{
    vec3 colorFinal = objectColor;

    vec2 uv;
    vec2 tamCara;

    // Elegir la cara del cubo
    if (abs(localPos.z) > 0.49)
    {
        uv = localPos.xy + vec2(0.5, 0.5);
        tamCara = vec2(objectScale.x, objectScale.y);
    }
    else if (abs(localPos.x) > 0.49)
    {
        uv = vec2(localPos.z, localPos.y) + vec2(0.5, 0.5);
        tamCara = vec2(objectScale.z, objectScale.y);
    }
    else
    {
        uv = vec2(localPos.x, localPos.z) + vec2(0.5, 0.5);
        tamCara = vec2(objectScale.x, objectScale.z);
    }

    vec2 superficie = uv * tamCara;
    float tamPixel = 0.30;
    vec2 cuadrito = floor(superficie / tamPixel);
    float valor = random(cuadrito);

    // =====================================================
    // POLLO BLANCO
    // =====================================================
    if (
        objectColor.r > 0.8 &&
        objectColor.g > 0.8 &&
        objectColor.b > 0.8
       )
    {
        if (valor < 0.55)
            colorFinal = vec3(0.96, 0.96, 0.96);
        else if (valor < 0.80)
            colorFinal = vec3(0.90, 0.90, 0.90);
        else if (valor < 0.94)
            colorFinal = vec3(0.84, 0.84, 0.84);
        else
            colorFinal = vec3(0.78, 0.78, 0.78);
    }

    // =====================================================
    // PASTO
    // =====================================================
    else if (
        objectColor.r > 0.25 && objectColor.r < 0.45 &&
        objectColor.g > 0.50 && objectColor.g < 0.75 &&
        objectColor.b > 0.10 && objectColor.b < 0.30
    )
    {
        if (valor < 0.45)
            colorFinal = vec3(0.36, 0.69, 0.20);
        else if (valor < 0.75)
            colorFinal = vec3(0.43, 0.76, 0.25);
        else if (valor < 0.92)
            colorFinal = vec3(0.31, 0.61, 0.17);
        else
            colorFinal = vec3(0.50, 0.82, 0.30);
    }

    // =====================================================
    // TIERRA
    // =====================================================
    else if (
        objectColor.r > 0.35 && objectColor.r < 0.55 &&
        objectColor.g > 0.20 && objectColor.g < 0.40 &&
        objectColor.b > 0.08 && objectColor.b < 0.22
    )
    {
        if (valor < 0.45)
            colorFinal = vec3(0.45, 0.30, 0.14);
        else if (valor < 0.75)
            colorFinal = vec3(0.52, 0.35, 0.18);
        else if (valor < 0.92)
            colorFinal = vec3(0.38, 0.25, 0.12);
        else
            colorFinal = vec3(0.60, 0.42, 0.22);
    }

    color = vec4(colorFinal, 1.0);
}