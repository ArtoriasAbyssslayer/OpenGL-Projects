#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Camera.h"
#include "BlackHole.h"
#include "AccretionDisk.h"
#include "Starfield.h"
#include "PostProcessor.h"
#include "RelativisticJet.h"
#include "GravitationalField.h"
#include "LensingGrid.h"
#include <memory>

class Renderer {
public:
    Renderer(unsigned int width = 1280, unsigned int height = 720);
    ~Renderer();

    // Render complete scene
    void render(const BlackHole& blackHole, const AccretionDisk& accretionDisk, 
                const Starfield& starfield, const RelativisticJet& jets,
                const GravitationalField& gravField, const LensingGrid& lensingGrid, 
                const Camera& camera, float deltaTime, float time);

    // Resize post-processing buffers
    void resize(unsigned int width, unsigned int height);

private:
    std::unique_ptr<Shader> blackHoleShader;
    std::unique_ptr<Shader> accretionDiskShader;
    std::unique_ptr<Shader> starfieldShader;
    std::unique_ptr<Shader> jetShader;
    std::unique_ptr<Shader> fieldShader;
    std::unique_ptr<Shader> gridShader;
    std::unique_ptr<PostProcessor> postProcessor;
    
    void initShaders();
};

#endif