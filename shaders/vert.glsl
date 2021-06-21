#version 420

layout (location = 0) in vec3 vp;
layout (location = 1) in float vColor;

out vec3 fColor;

void main(){
	fColor = vec3(vColor, 0.0, 0.5);
	gl_Position = vec4(vp, 1.0);
}