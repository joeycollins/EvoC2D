#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
out vec3 vertexColor;
uniform mat4 transform;
void main(){
	gl_Position = transform * vec4(aPos.xyz, 1.0f);
	vertexColor = vec3(1.0,0.0,0.0);
};