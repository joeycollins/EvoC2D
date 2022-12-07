#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vertexColor;
uniform mat4 projection;
uniform vec2 offset;
uniform vec2 scaling;
void main()
{
    gl_Position = projection * vec4(scaling.x * aPos.x + offset.x,scaling.y * aPos.y + offset.y, 0.0f, 1.0);
    vertexColor = aColor;
}