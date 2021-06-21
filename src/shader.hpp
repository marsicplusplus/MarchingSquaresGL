#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "glad/glad.h"

#include <string>
#include <vector>

class Shader {
	public:
		Shader();
		~Shader();

		void use();
		void compileShaders();
		unsigned int getID() const;

		int loadShader(const char* filePath, int type);
		int checkCompileErrors(unsigned int shader, std::string type);

	private:
		unsigned int programID;
		std::vector<int> shadersID;

};

#endif
