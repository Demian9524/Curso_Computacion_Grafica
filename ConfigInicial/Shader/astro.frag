#version 330 core
in vec2 UV;
in float worldHeight;
out vec4 color;
uniform bool isMoon;
// Disenos originales de 16x16 inspirados en Minecraft.
// No requieren imagenes ni descargan texturas del juego.
bool rect(vec2 p, vec2 a, vec2 b) {
    return all(greaterThanEqual(p, a)) && all(lessThan(p, b));
}
void main()
{
    if (worldHeight < 0.0) discard; // El astro se oculta bajo el horizonte.
    vec2 p = min(floor(UV * 16.0), vec2(15.0));
    float edge = min(min(p.x, p.y), min(15.0-p.x, 15.0-p.y));
    vec3 c;
    if (!isMoon) {
        c = vec3(1.0, 0.68, 0.16);
        if (edge >= 1.0) c = vec3(1.0, 0.87, 0.40);
        if (edge >= 3.0) c = vec3(1.0, 0.97, 0.77);
        if (edge >= 5.0) c = vec3(1.0, 1.0, 0.92);
        if (rect(p, vec2(3,10), vec2(5,13))) c = vec3(1.0,0.93,0.62);
    } else {
        c = edge < 1.0 ? vec3(0.59,0.65,0.75) : vec3(0.88,0.92,1.0);
        if (rect(p,vec2(3,9),vec2(7,13)) || rect(p,vec2(9,3),vec2(13,6))
            || rect(p,vec2(3,3),vec2(5,5))) c = vec3(0.60,0.67,0.78);
        if (rect(p,vec2(4,10),vec2(6,12)) || rect(p,vec2(10,4),vec2(12,6)))
            c = vec3(0.44,0.51,0.65);
        if (rect(p,vec2(10,11),vec2(13,13))) c = vec3(0.76,0.82,0.91);
    }
    // Color emisivo: el sol y la luna no se oscurecen con la escena.
    color = vec4(c, 1.0);
}
