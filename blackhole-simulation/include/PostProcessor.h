#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <memory>
#include "Shader.h"

class PostProcessor
{
public:
    // Framebuffer objects
    unsigned int hdrFBO, colorBuffers[2], rboDepth;
    unsigned int pingpongFBO[2], pingpongColorbuffer[2];
    unsigned int quadVAO, quadVBO;
    
    // Shaders
    std::unique_ptr<Shader> hdrShader;
    std::unique_ptr<Shader> blurShader;
    
    // Configuration
    bool hdrEnabled;
    bool bloomEnabled;
    float exposure;
    float bloomStrength;
    
    // Constructor
    PostProcessor(unsigned int width, unsigned int height);
    
    // Destructor
    ~PostProcessor();
    
    // Begin HDR rendering
    void beginRender();
    
    // End HDR rendering and apply post-processing
    void endRender();
    
    // Resize buffers
    void resize(unsigned int width, unsigned int height);
    
private:
    unsigned int width, height;
    
    void setupShaders();
    void setupGeometry();
    void setupFramebuffers();
    void renderBlur();
};

#endif //POSTPROCESSOR_H