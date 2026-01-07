#include "Starfield.h"
#include <cmath>

Starfield::Starfield(unsigned int numStars, float radius) 
    : rng(std::random_device{}()), VAO(0), VBO(0) {
    generateStars(numStars, radius);
    setupMesh();
}

Starfield::~Starfield() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Starfield::generateStars(unsigned int numStars, float radius) {
    stars.clear();
    vertices.clear();
    
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> heightDist(-radius, radius);
    std::uniform_real_distribution<float> brightnessDist(0.3f, 1.0f);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);
    std::uniform_real_distribution<float> colorDist(0.7f, 1.0f);
    
    for (unsigned int i = 0; i < numStars; ++i) {
        Star star;
        
        // Random spherical position
        float theta = angleDist(rng);
        float phi = acos(1.0f - 2.0f * (float)i / numStars);
        float r = radius * (0.8f + 0.2f * (float)i / numStars);
        
        star.position = glm::vec3(
            r * sin(phi) * cos(theta),
            r * cos(phi),
            r * sin(phi) * sin(theta)
        );
        
        star.brightness = brightnessDist(rng);
        star.size = sizeDist(rng);
        
        // Star colors (white to blue-white)
        star.color = glm::vec3(
            colorDist(rng) * 0.9f,
            colorDist(rng),
            colorDist(rng)
        );
        
        stars.push_back(star);
        
        // Add to vertex data
        vertices.push_back(star.position.x);
        vertices.push_back(star.position.y);
        vertices.push_back(star.position.z);
        vertices.push_back(star.brightness);
        vertices.push_back(star.size);
        vertices.push_back(star.color.r);
        vertices.push_back(star.color.g);
        vertices.push_back(star.color.b);
    }
}

void Starfield::setupMesh() {
    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Brightness attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    // Size attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(4 * sizeof(float)));
    
    // Color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(5 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);
}