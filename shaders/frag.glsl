#version 420

in vec3 fColor;

out vec4 fragCol;

void main(){
	fragCol = vec4(fColor, 1.0);
}
