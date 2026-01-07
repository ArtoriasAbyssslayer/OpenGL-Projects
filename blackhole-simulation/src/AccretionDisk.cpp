#include "AccretionDisk.h"
#include <glad/glad.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AccretionDisk::AccretionDisk(const glm::vec3& pos, float innerRad, float outerRad, const glm::vec3& axis)
    : position(pos), innerRadius(innerRad), outerRadius(outerRad), rotationAxis(axis),
      VAO(0), VBO(0), EBO(0) {
    generateDisk(innerRadius, outerRadius, 128);
    setupMesh();
}

AccretionDisk::~AccretionDisk() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void AccretionDisk::generateDisk(float innerRadius, float outerRadius, unsigned int sectors) {
    vertices.clear();
    indices.clear();
    
    // Generate vertices for disk
    for (unsigned int s = 0; s <= sectors; ++s) {
        float angle = 2.0f * M_PI * (float)s / (float)sectors;
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        
        // Inner ring vertex
        vertices.push_back(innerRadius * cosAngle);
        vertices.push_back(0.0f);
        vertices.push_back(innerRadius * sinAngle);
        
        // Normal (pointing up)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Texture coordinates
        vertices.push_back((float)s / (float)sectors);
        vertices.push_back(0.0f);
        
        // Outer ring vertex
        vertices.push_back(outerRadius * cosAngle);
        vertices.push_back(0.0f);
        vertices.push_back(outerRadius * sinAngle);
        
        // Normal (pointing up)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);
        
        // Texture coordinates
        vertices.push_back((float)s / (float)sectors);
        vertices.push_back(1.0f);
    }
    
    // Generate indices for triangles
    for (unsigned int s = 0; s < sectors; ++s) {
        unsigned int current = s * 2;
        unsigned int next = ((s + 1) % sectors) * 2;
        
        // First triangle
        indices.push_back(current);
        indices.push_back(next);
        indices.push_back(current + 1);
        
        // Second triangle
        indices.push_back(next);
        indices.push_back(next + 1);
        indices.push_back(current + 1);
    }
}

void AccretionDisk::setupMesh() {
    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind VAO
    glBindVertexArray(VAO);
    
    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Load data into element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 indices.data(), GL_STATIC_DRAW);
    
    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);
}