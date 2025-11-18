#include "shaderClass.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

std::string get_file_contents(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);
    
    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file: " << filename << std::endl;
        std::cerr << "Error code: " << errno << std::endl;
        throw std::runtime_error(std::string("Failed to open file: ") + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string contents = buffer.str();
    
    if (contents.empty())
    {
        std::cerr << "WARNING: File is empty: " << filename << std::endl;
    }
    
    std::cout << "Successfully loaded file: " << filename 
              << " (" << contents.length() << " bytes)" << std::endl;
    
    return contents;
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    std::cout << "Loading shaders..." << std::endl;
    
    // Read shader files
    std::string vertexCode;
    std::string fragmentCode;
    
    try {
        vertexCode = get_file_contents(vertexFile);
        fragmentCode = get_file_contents(fragmentFile);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to read shader files: " << e.what() << std::endl;
        throw;
    }

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Compile vertex shader
    std::cout << "Compiling vertex shader..." << std::endl;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        std::cerr << "ERROR: Vertex Shader Compilation Failed\n" << infoLog << std::endl;
        throw std::runtime_error("Vertex shader compilation failed");
    }
    std::cout << "Vertex shader compiled successfully" << std::endl;

    // Compile fragment shader
    std::cout << "Compiling fragment shader..." << std::endl;
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        std::cerr << "ERROR: Fragment Shader Compilation Failed\n" << infoLog << std::endl;
        
        // Print first few lines of shader for debugging
        std::stringstream ss(fragmentCode);
        std::string line;
        int lineNum = 1;
        std::cerr << "\nFirst 20 lines of fragment shader:" << std::endl;
        while (std::getline(ss, line) && lineNum <= 20) {
            std::cerr << lineNum << ": " << line << std::endl;
            lineNum++;
        }
        
        throw std::runtime_error("Fragment shader compilation failed");
    }
    std::cout << "Fragment shader compiled successfully" << std::endl;

    // Link shaders
    std::cout << "Linking shader program..." << std::endl;
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertexShader);
    glAttachShader(shader_program_id, fragmentShader);
    glLinkProgram(shader_program_id);
    
    // Check for linking errors
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_id, 1024, NULL, infoLog);
        std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
        throw std::runtime_error("Shader linking failed");
    }
    std::cout << "Shader program linked successfully" << std::endl;

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
    glUseProgram(shader_program_id);
}

void Shader::Delete()
{
    glDeleteProgram(shader_program_id);
}