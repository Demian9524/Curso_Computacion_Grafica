#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
out vec2 UV;
out float worldHeight;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    UV = texCoords;
    vec4 world = model * vec4(position, 1.0);
    worldHeight = world.y;
    gl_Position = projection * view * world;
}
