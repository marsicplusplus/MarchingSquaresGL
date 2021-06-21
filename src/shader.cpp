#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(){
	programID = glCreateProgram();
}

unsigned int Shader::getID() const {
	return programID;
}

int Shader::loadShader(const char* filePath, int type){
	std::string code;
	std::ifstream shaderFile;
	std::stringstream shaderStream;
	
	shaderFile.open(filePath);
	if(!shaderStream){
		std::cerr << "OPENSDL::SHADER::Shader::Cannot load shader file: " << filePath << std::endl;
		exit(1);
	}
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();

	code = shaderStream.str();

	const char* codeChar = code.c_str();
	unsigned int shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &codeChar, NULL);
	glCompileShader(shaderID);
	std::string typeS;
	switch (type) {
		case GL_VERTEX_SHADER:
			typeS = "VERTEX";
			break;
		case GL_FRAGMENT_SHADER:
			typeS = "FRAGMENT";
			break;
	}
	checkCompileErrors(shaderID, typeS);

	glAttachShader(programID, shaderID);
	
	shadersID.push_back(shaderID);

	return 0;
}

Shader::~Shader(){
	glDeleteProgram(programID);
}

void Shader::compileShaders(){
	glLinkProgram(programID);
	checkCompileErrors(programID, "PROGRAM");

	for(unsigned int shaderID : shadersID){
		glDeleteShader(shaderID);
	}
}

void Shader::use(){
	glUseProgram(programID);
}

int Shader::checkCompileErrors(unsigned int shader, std::string type) {
	int success;
	char infoLog[1024];
	if(type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return -1;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			return -1;
		}
	}
	return 0;
}
