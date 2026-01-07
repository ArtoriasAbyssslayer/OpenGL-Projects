#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer(unsigned int width, unsigned int height) {
    postProcessor = std::make_unique<PostProcessor>(width, height);
    initShaders();
}

Renderer::~Renderer() {
    // Shader cleanup handled by unique_ptr
}

void Renderer::initShaders() {
    blackHoleShader = std::make_unique<Shader>("shaders/vertex.glsl", 
                                                "shaders/fragment.glsl");
    accretionDiskShader = std::make_unique<Shader>("shaders/vertex.glsl", 
                                                   "shaders/accretion_fragment.glsl");
    starfieldShader = std::make_unique<Shader>("shaders/starfield_vertex.glsl", 
                                               "shaders/starfield_fragment.glsl");
    jetShader = std::make_unique<Shader>("shaders/jet_vertex.glsl", 
                                         "shaders/jet_fragment.glsl");
    fieldShader = std::make_unique<Shader>("shaders/field_vertex.glsl", 
                                         "shaders/field_fragment.glsl");
    gridShader = std::make_unique<Shader>("shaders/field_vertex.glsl", 
                                       "shaders/field_fragment.glsl");
}

void Renderer::resize(unsigned int width, unsigned int height) {
    postProcessor->resize(width, height);
}

void Renderer::render(const BlackHole& blackHole, const AccretionDisk& accretionDisk, 
                      const Starfield& starfield, const RelativisticJet& jets,
                      const GravitationalField& gravField, const LensingGrid& lensingGrid, 
                      const Camera& camera, float deltaTime, float time) {
    
    // Begin HDR rendering
    postProcessor->beginRender();
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

    // Render starfield first (background)
    starfieldShader->use();
    
    glm::mat4 starModel = glm::mat4(1.0f);
    
    starfieldShader->setMat4("model", starModel);
    starfieldShader->setMat4("view", view);
    starfieldShader->setMat4("projection", projection);
    starfieldShader->setVec3("blackHolePos", blackHole.getPosition());
    starfieldShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    starfieldShader->setVec3("viewPos", camera.Position);
    
    // Enable blending for stars
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(starfield.getVAO());
    glDrawArrays(GL_POINTS, 0, starfield.getStarCount());
    glBindVertexArray(0);
    
    // Render relativistic jets
    jetShader->use();
    
    glm::mat4 jetModel = glm::mat4(1.0f);
    
    jetShader->setMat4("model", jetModel);
    jetShader->setMat4("view", view);
    jetShader->setMat4("projection", projection);
    jetShader->setVec3("blackHolePos", blackHole.getPosition());
    jetShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    jetShader->setVec3("viewPos", camera.Position);
    
    // Render relativistic jets (temporarily disabled)
    /*
    jetShader->use();
    
    jetShader->setMat4("model", jetModel);
    jetShader->setMat4("view", view);
    jetShader->setMat4("projection", projection);
    jetShader->setVec3("blackHolePos", blackHole.getPosition());
    jetShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    jetShader->setVec3("viewPos", camera.Position);
    
    // Update jet particle system
    const_cast<RelativisticJet&>(jets).update(deltaTime);
    
    // Update vertex buffer with new particle data
    const_cast<RelativisticJet&>(jets).updateBuffer();
    
    glBindVertexArray(jets.getVAO());
    glDrawArrays(GL_POINTS, 0, jets.getParticleCount());
    glBindVertexArray(0);
    */
    
    // Render accretion disk
    accretionDiskShader->use();
    
    glm::mat4 diskModel = glm::mat4(1.0f);
    diskModel = glm::translate(diskModel, accretionDisk.getPosition());
    diskModel = glm::rotate(diskModel, glm::radians(75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    accretionDiskShader->setMat4("model", diskModel);
    accretionDiskShader->setMat4("view", view);
    accretionDiskShader->setMat4("projection", projection);
    accretionDiskShader->setVec3("blackHolePos", blackHole.getPosition());
    accretionDiskShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    accretionDiskShader->setFloat("innerRadius", accretionDisk.getInnerRadius());
    accretionDiskShader->setFloat("outerRadius", accretionDisk.getOuterRadius());
    accretionDiskShader->setVec3("viewPos", camera.Position);
    accretionDiskShader->setFloat("time", time);
    
    glBindVertexArray(accretionDisk.getVAO());
    glDrawElements(GL_TRIANGLES, accretionDisk.getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Render black hole
    blackHoleShader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, blackHole.getPosition());

    blackHoleShader->setMat4("model", model);
    blackHoleShader->setMat4("view", view);
    blackHoleShader->setMat4("projection", projection);
    blackHoleShader->setVec3("blackHolePos", blackHole.getPosition());
    blackHoleShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    blackHoleShader->setVec3("viewPos", camera.Position);

    // Render the black hole sphere
    glBindVertexArray(blackHole.getVAO());
    glDrawElements(GL_TRIANGLES, blackHole.getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    // Disable blending
    glDisable(GL_BLEND);
    
    // Render gravitational field boundaries (NASA-style)
    fieldShader->use();
    
    glm::mat4 fieldModel = glm::mat4(1.0f);
    
    fieldShader->setMat4("model", fieldModel);
    fieldShader->setMat4("view", view);
    fieldShader->setMat4("projection", projection);
    fieldShader->setVec3("blackHolePos", blackHole.getPosition());
    fieldShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    fieldShader->setVec3("viewPos", camera.Position);
    
    // Enable blending for field lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(gravField.getVAO());
    glDrawArrays(GL_LINES, 0, gravField.getVertexCount());
    glBindVertexArray(0);
    
    // Render gravitational lensing grid (simplified for testing)
    /*
    gridShader->use();
    
    glm::mat4 gridModel = glm::mat4(1.0f);
    
    gridShader->setMat4("model", gridModel);
    gridShader->setMat4("view", view);
    gridShader->setMat4("projection", projection);
    gridShader->setVec3("blackHolePos", blackHole.getPosition());
    gridShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    gridShader->setVec3("viewPos", camera.Position);
    
    // Update lensing grid distortion
    const_cast<LensingGrid&>(lensingGrid).updateDistortion(camera.Position);
    
    // Enable blending for grid lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(lensingGrid.getVAO());
    glDrawArrays(GL_LINES, 0, lensingGrid.getVertexCount());
    glBindVertexArray(0);
    */
    
    // Apply post-processing effects (HDR, bloom, etc.)
    postProcessor->endRender();
}