#include "BlackHole.h"
#include <glad/glad.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BlackHole::BlackHole(const glm::vec3& pos, float m) 
    : position(pos), mass(m), VAO(0), VBO(0), EBO(0) {
    calculateRadii();
    generateSphere(schwarzschildRadius, 64, 64);
    setupMesh();
}

BlackHole::~BlackHole() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void BlackHole::calculateRadii() {
    // Simplified calculation for visualization
    // In reality: Rs = 2GM/c^2, but we'll use simplified values
    schwarzschildRadius = mass * 0.5f;  // Event horizon
    photonSphereRadius = schwarzschildRadius * 1.5f;  // Photon sphere
}

void BlackHole::generateSphere(float radius, unsigned int rings, unsigned int sectors) {
    vertices.clear();
    indices.clear();

    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);

    // Generate vertices
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            float const y = sin(-M_PI / 2.0f + M_PI * r * R);
            float const x = cos(2.0f * M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2.0f * M_PI * s * S) * sin(M_PI * r * R);

            // Position
            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);

            // Normal
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Texture coordinates
            vertices.push_back(s * S);
            vertices.push_back(r * R);
        }
    }

    // Generate indices
    for (unsigned int r = 0; r < rings - 1; ++r) {
        for (unsigned int s = 0; s < sectors - 1; ++s) {
            unsigned int curRow = r * sectors;
            unsigned int nextRow = (r + 1) * sectors;

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s);
            indices.push_back(nextRow + (s + 1));

            indices.push_back(curRow + s);
            indices.push_back(nextRow + (s + 1));
            indices.push_back(curRow + (s + 1));
        }
    }
}

void BlackHole::setupMesh() {
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