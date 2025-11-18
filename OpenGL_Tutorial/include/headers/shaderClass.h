#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint shader_program_id;
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
	GLuint GetID() const {return shader_program_id; };
};

#endif // SHADER_CLASS_H 