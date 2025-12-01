#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Camera.h"
#include "BlackHole.h"
#include <memory>

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Render the black hole
    void render(const BlackHole& blackHole, const Camera& camera);

private:
    std::unique_ptr<Shader> blackHoleShader;
    
    void initShaders();
};

#endif