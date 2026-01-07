#include "GravitationalField.h"
#include <cmath>
#include <algorithm>

GravitationalField::GravitationalField(const glm::vec3& bhPos, float bhRadius, float maxFieldRadius)
    : blackHolePos(bhPos), schwarzschildRadius(bhRadius), maxRadius(maxFieldRadius),
      VAO(0), VBO(0), rng(std::random_device{}()) {
    generateFieldLines();
    generateContours();
    updateVertices();
    setupMesh();
}

GravitationalField::~GravitationalField() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

float GravitationalField::calculatePotential(const glm::vec3& pos) {
    float dist = length(pos - blackHolePos);
    if (dist < schwarzschildRadius) {
        return -1.0f; // Inside event horizon
    }
    // Simplified gravitational potential
    return -schwarzschildRadius / dist;
}

glm::vec3 GravitationalField::calculateFieldStrength(const glm::vec3& pos) {
    glm::vec3 r = pos - blackHolePos;
    float dist = length(r);
    if (dist < schwarzschildRadius) {
        return glm::vec3(0.0f); // No field direction inside
    }
    
    // Field strength follows inverse square law
    float strength = schwarzschildRadius * schwarzschildRadius / (dist * dist * dist);
    return -normalize(r) * strength;
}

void GravitationalField::traceFieldLine(const glm::vec3& startDir, float startRadius) {
    FieldLine line;
    line.points.clear();
    
    glm::vec3 pos = blackHolePos + startDir * startRadius;
    line.color = glm::vec3(0.3f, 0.5f, 1.0f); // Blue field lines
    
    float step = 0.1f;
    int maxSteps = 500;
    
    for (int i = 0; i < maxSteps; ++i) {
        line.points.push_back(pos);
        
        // Check if we're too close or too far
        float dist = length(pos - blackHolePos);
        if (dist < schwarzschildRadius * 1.1f || dist > maxRadius) {
            break;
        }
        
        // Trace field line
        glm::vec3 field = calculateFieldStrength(pos);
        pos += normalize(field) * step;
    }
    
    // Calculate line strength based on distance from black hole
    if (!line.points.empty()) {
        float avgDist = 0.0f;
        for (const auto& point : line.points) {
            avgDist += length(point - blackHolePos);
        }
        avgDist /= line.points.size();
        line.strength = 1.0f - (avgDist / maxRadius);
    }
    
    if (line.points.size() > 1) {
        fieldLines.push_back(line);
    }
}

void GravitationalField::generateFieldLines() {
    fieldLines.clear();
    
    // Generate field lines in spherical pattern
    int numLatLines = 8;
    int numLongLines = 16;
    float startRadius = schwarzschildRadius * 2.0f;
    
    // Latitude lines
    for (int lat = 0; lat < numLatLines; ++lat) {
        float theta = (float)lat / (float)numLatLines * M_PI;
        
        for (int lon = 0; lon < numLongLines; ++lon) {
            float phi = (float)lon / (float)numLongLines * 2.0f * M_PI;
            
            glm::vec3 dir(
                sin(theta) * cos(phi),
                cos(theta),
                sin(theta) * sin(phi)
            );
            
            traceFieldLine(dir, startRadius);
        }
    }
}

void GravitationalField::generateContour(float radius, const glm::vec3& color, float alpha) {
    int segments = 64;
    
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / (float)segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / (float)segments * 2.0f * M_PI;
        
        // Point on the contour
        glm::vec3 p1(
            blackHolePos.x + radius * cos(angle1),
            blackHolePos.y,
            blackHolePos.z + radius * sin(angle1)
        );
        
        glm::vec3 p2(
            blackHolePos.x + radius * cos(angle2),
            blackHolePos.y,
            blackHolePos.z + radius * sin(angle2)
        );
        
        // Add line segment
        vertices.push_back(p1.x);
        vertices.push_back(p1.y);
        vertices.push_back(p1.z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(alpha);
        
        vertices.push_back(p2.x);
        vertices.push_back(p2.y);
        vertices.push_back(p2.z);
        vertices.push_back(color.r);
        vertices.push_back(color.g);
        vertices.push_back(color.b);
        vertices.push_back(alpha);
    }
}

void GravitationalField::generateContours() {
    // Clear existing contour vertices
    vertices.clear();
    
    // Event horizon contour (red, solid)
    generateContour(schwarzschildRadius, glm::vec3(1.0f, 0.2f, 0.2f), 1.0f);
    
    // Photon sphere contour (orange, bright)
    float photonSphere = schwarzschildRadius * 1.5f;
    generateContour(photonSphere, glm::vec3(1.0f, 0.6f, 0.2f), 0.8f);
    
    // Innermost stable circular orbit (3x Schwarzschild radius)
    float isco = schwarzschildRadius * 3.0f;
    generateContour(isco, glm::vec3(1.0f, 0.8f, 0.3f), 0.6f);
    
    // Additional influence contours
    for (int i = 4; i <= 8; ++i) {
        float radius = schwarzschildRadius * (float)i;
        float intensity = 1.0f - (float)(i - 4) / 4.0f;
        glm::vec3 color = glm::vec3(0.2f, 0.4f + intensity * 0.3f, 0.8f + intensity * 0.2f);
        generateContour(radius, color, intensity * 0.4f);
    }
}

void GravitationalField::updateVertices() {
    vertices.clear();
    
    // Add all contour vertices first
    generateContours();
    
    // Add field lines
    for (const auto& line : fieldLines) {
        for (size_t i = 0; i < line.points.size() - 1; ++i) {
            const auto& p1 = line.points[i];
            const auto& p2 = line.points[i + 1];
            
            // Line segment with color based on field strength
            vertices.push_back(p1.x);
            vertices.push_back(p1.y);
            vertices.push_back(p1.z);
            vertices.push_back(line.color.r);
            vertices.push_back(line.color.g);
            vertices.push_back(line.color.b);
            vertices.push_back(line.strength * 0.5f);
            
            vertices.push_back(p2.x);
            vertices.push_back(p2.y);
            vertices.push_back(p2.z);
            vertices.push_back(line.color.r);
            vertices.push_back(line.color.g);
            vertices.push_back(line.color.b);
            vertices.push_back(line.strength * 0.5f);
        }
    }
}

void GravitationalField::setupMesh() {
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    
    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    
    // Alpha attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 
                         (void*)(6 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);
}