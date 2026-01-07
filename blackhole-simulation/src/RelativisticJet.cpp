#include "RelativisticJet.h"
#include <cmath>

RelativisticJet::RelativisticJet(const glm::vec3& bhPos, float bhRadius)
    : blackHolePos(bhPos), schwarzschildRadius(bhRadius), 
      jetPower(1.0f), jetAngle(0.1f), time(0.0f), VAO(0), VBO(0),
      rng(std::random_device{}()) {
    
    generateParticles();
    setupMesh();
}

RelativisticJet::~RelativisticJet() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void RelativisticJet::generateParticles() {
    particles.clear();
    
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> velocityDist(0.8f, 1.2f);
    std::uniform_real_distribution<float> lifetimeDist(2.0f, 5.0f);
    
    // Generate particles for upper and lower jets
    for (int i = 0; i < 500; ++i) {
        particles.push_back(createParticle(true));  // Upper jet
        particles.push_back(createParticle(false)); // Lower jet
    }
}

RelativisticJet::JetParticle RelativisticJet::createParticle(bool upperJet) {
    JetParticle particle;
    
    // Start near the poles of black hole
    std::uniform_real_distribution<float> startAngle(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> startRadius(schwarzschildRadius * 1.1f, schwarzschildRadius * 1.5f);
    std::uniform_real_distribution<float> velocityDist(0.8f, 1.2f);
    std::uniform_real_distribution<float> lifetimeDist(2.0f, 5.0f);
    
    float angle = startAngle(rng);
    float radius = startRadius(rng);
    
    particle.position = blackHolePos;
    particle.position.x += cos(angle) * radius * 0.1f;
    particle.position.z += sin(angle) * radius * 0.1f;
    particle.position.y += (upperJet ? 1.0f : -1.0f) * schwarzschildRadius * 0.5f;
    
    // Initial velocity - mostly along Y axis with some spread
    std::uniform_real_distribution<float> spreadDist(-jetAngle, jetAngle);
    particle.velocity = glm::vec3(
        spreadDist(rng),
        (upperJet ? 1.0f : -1.0f) * velocityDist(rng),
        spreadDist(rng)
    );
    particle.velocity *= jetPower * 2.0f;
    
    // Particle properties
    particle.lifetime = 0.0f;
    particle.maxLifetime = lifetimeDist(rng);
    
    // Temperature based on distance from black hole
    float tempFactor = 1.0f - (radius / (schwarzschildRadius * 3.0f));
    particle.temperature = tempFactor;
    
    // Color based on temperature (blue-white hot plasma)
    if (particle.temperature > 0.8f) {
        particle.color = glm::vec3(0.7f, 0.8f, 1.0f);
    } else if (particle.temperature > 0.6f) {
        particle.color = glm::vec3(0.9f, 0.9f, 1.0f);
    } else {
        particle.color = glm::vec3(1.0f, 0.7f, 0.5f);
    }
    
    particle.size = (0.5f + tempFactor * 1.5f) * 2.0f;
    
    return particle;
}

void RelativisticJet::update(float deltaTime) {
    time += deltaTime;
    
    for (auto& particle : particles) {
        // Update lifetime
        particle.lifetime += deltaTime;
        
        // Respawn particle if it's too old
        if (particle.lifetime >= particle.maxLifetime) {
            bool upperJet = particle.position.y > blackHolePos.y;
            particle = createParticle(upperJet);
            continue;
        }
        
        // Update position
        particle.position += particle.velocity * deltaTime;
        
        // Apply relativistic acceleration (simplified)
        float distFromCenter = length(particle.position - blackHolePos);
        if (distFromCenter > schwarzschildRadius * 2.0f) {
            // Acceleration along jet axis
            float acceleration = jetPower / (distFromCenter * 0.5f);
            particle.velocity.y += (particle.velocity.y > 0 ? acceleration : -acceleration) * deltaTime;
        }
        
        // Add some turbulence
        std::uniform_real_distribution<float> turbulence(-0.1f, 0.1f);
        particle.velocity.x += turbulence(rng) * deltaTime;
        particle.velocity.z += turbulence(rng) * deltaTime;
        
        // Update temperature (cooling over time)
        particle.temperature *= (1.0f - deltaTime * 0.1f);
        
        // Update size based on lifetime (expansion)
        float lifeFactor = particle.lifetime / particle.maxLifetime;
        particle.size = (0.5f + particle.temperature) * (1.0f + lifeFactor * 2.0f);
        
        // Fade color as it cools
        if (particle.temperature < 0.5f) {
            particle.color = glm::mix(glm::vec3(1.0f, 0.3f, 0.1f), particle.color, 
                                   particle.temperature * 2.0f);
        }
    }
    
    updateVertices();
}

void RelativisticJet::updateVertices() {
    vertices.clear();
    
    for (const auto& particle : particles) {
        // Position
        vertices.push_back(particle.position.x);
        vertices.push_back(particle.position.y);
        vertices.push_back(particle.position.z);
        
        // Color
        vertices.push_back(particle.color.r);
        vertices.push_back(particle.color.g);
        vertices.push_back(particle.color.b);
        
        // Size
        vertices.push_back(particle.size);
        
        // Lifetime (for alpha)
        float lifeFactor = 1.0f - (particle.lifetime / particle.maxLifetime);
        vertices.push_back(lifeFactor * particle.temperature);
        
        // Temperature
        vertices.push_back(particle.temperature);
    }
}

void RelativisticJet::setupMesh() {
    updateVertices();
    
    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_DYNAMIC_DRAW);
    
    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    // Size attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    
    // Alpha/Intensity attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(7 * sizeof(float)));
    
    // Temperature attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 
                         (void*)(8 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);
}

void RelativisticJet::updateBuffer() {
    updateVertices();
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}