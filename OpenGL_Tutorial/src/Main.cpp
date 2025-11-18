#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

/* GLOBALS */

// Fullscreen quad vertices (position only, no colors needed)
GLfloat vertices[] =
{
    // positions        
    -1.0f,  1.0f, 0.0f,  // top left
    -1.0f, -1.0f, 0.0f,  // bottom left
     1.0f, -1.0f, 0.0f,  // bottom right
     1.0f,  1.0f, 0.0f   // top right
};

// Indices for the quad (two triangles)
GLuint indices[] =
{
    0, 1, 2,  // first triangle
    0, 2, 3   // second triangle
};

// Mouse position tracking
double mouseX = 0.0, mouseY = 0.0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int windowWidth = 720;
    int windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Raymarching Shader", NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, windowWidth, windowHeight);
    
    // Load shaders - wrap in try-catch
    Shader* shaderProgram = nullptr;
    try {
        shaderProgram = new Shader("default.vert", "fragment.glsl");
        std::cout << "Shaders compiled successfully!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Shader compilation failed: " << e.what() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown shader compilation error" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return -1;
    }

    // Setup VAO/VBO/EBO
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    // Link only position attribute (location 0, 3 floats, stride 3*float)
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    // Get uniform locations
    GLuint resolutionLoc = glGetUniformLocation(shaderProgram->shader_program_id, "iResolution");
    GLuint timeLoc = glGetUniformLocation(shaderProgram->shader_program_id, "iTime");
    GLuint mouseLoc = glGetUniformLocation(shaderProgram->shader_program_id, "iMouse");

    std::cout << "Uniform locations - iResolution: " << resolutionLoc 
              << ", iTime: " << timeLoc 
              << ", iMouse: " << mouseLoc << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Activate shader
        shaderProgram->Activate();
        
        // Update uniforms
        float time = (float)glfwGetTime();
        glUniform2f(resolutionLoc, (float)windowWidth, (float)windowHeight);
        glUniform1f(timeLoc, time);
        glUniform4f(mouseLoc, (float)mouseX, (float)mouseY, 0.0f, 0.0f);
        
        // Draw fullscreen quad
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram->Delete();
    delete shaderProgram;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}