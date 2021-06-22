#version 420 core

in vec3 fColor;

out vec4 frag_color;

void main(void) {
	frag_color = vec4(0.0, 1.0, 0.0, 1.0);
	//frag_color = vec4(fColor, 1.0);
}
