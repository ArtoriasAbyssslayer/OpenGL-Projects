#ifndef RELATIVISTICJET_H
#define RELATIVISTICJET_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <random>

class RelativisticJet
{
public:
    // Jet particle properties
    struct JetParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 color;
        float size;
        float lifetime;
        float maxLifetime;
        float temperature;
    };
    
    // Rendering data
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    std::vector<JetParticle> particles;
    
    // Jet properties
    glm::vec3 blackHolePos;
    float schwarzschildRadius;
    float jetPower;
    float jetAngle;
    
    // Constructor
    RelativisticJet(const glm::vec3& bhPos, float bhRadius);
    
    // Destructor
    ~RelativisticJet();
    
    // Update particle system
    void update(float deltaTime);
    
    // Initialize geometry
    void setupMesh();
    
    // Generate jet particles
    void generateParticles();
    
    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getParticleCount() const { return particles.size(); }
    const float* getVertexData() const { return vertices.data(); }
    void updateBuffer();
    unsigned int getVBO() const { return VBO; }
    
private:
    std::mt19937 rng;
    float time;
    
    void updateVertices();
    JetParticle createParticle(bool upperJet);
};

#endif //RELATIVISTICJET_H