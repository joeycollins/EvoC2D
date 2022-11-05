#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 vertexColor;
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 aColor;

void main() {
	gl_Position = projection * view * transform * vec4(aPos.xyz, 1.0f);
	vertexColor = aColor;
};