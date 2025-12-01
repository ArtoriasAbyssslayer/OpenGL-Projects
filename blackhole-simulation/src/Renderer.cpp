#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    initShaders();
}

Renderer::~Renderer() {
    // Shader cleanup handled by unique_ptr
}

void Renderer::initShaders() {
    blackHoleShader = std::make_unique<Shader>("shaders/vertex.glsl", 
                                                "shaders/fragment.glsl");
}

void Renderer::render(const BlackHole& blackHole, const Camera& camera) {
    // Use shader
    blackHoleShader->use();

    // Set up transformation matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, blackHole.getPosition());

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);

    // Send matrices to shader
    blackHoleShader->setMat4("model", model);
    blackHoleShader->setMat4("view", view);
    blackHoleShader->setMat4("projection", projection);

    // Send black hole properties
    blackHoleShader->setVec3("blackHolePos", blackHole.getPosition());
    blackHoleShader->setFloat("schwarzschildRadius", blackHole.getSchwarzschildRadius());
    blackHoleShader->setVec3("viewPos", camera.Position);

    // Render the black hole sphere
    glBindVertexArray(blackHole.getVAO());
    glDrawElements(GL_TRIANGLES, blackHole.getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}