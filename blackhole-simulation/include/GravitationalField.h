#ifndef GRAVITATIONALFIELD_H
#define GRAVITATIONALFIELD_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <random>

class GravitationalField
{
public:
    // Field line properties
    struct FieldLine {
        std::vector<glm::vec3> points;
        float strength;
        glm::vec3 color;
    };
    
    // Rendering data
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    std::vector<FieldLine> fieldLines;
    
    // Field properties
    glm::vec3 blackHolePos;
    float schwarzschildRadius;
    float maxRadius;
    
    // Constructor
    GravitationalField(const glm::vec3& bhPos, float bhRadius, float maxFieldRadius = 20.0f);
    
    // Destructor
    ~GravitationalField();
    
    // Generate field visualization
    void generateFieldLines();
    
    // Generate contour circles for boundaries
    void generateContours();
    
    // Update rendering data
    void updateVertices();
    
    // Initialize geometry
    void setupMesh();
    
    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getVertexCount() const { return vertices.size() / 3; }
    
private:
    std::mt19937 rng;
    
    // Calculate gravitational potential at a point
    float calculatePotential(const glm::vec3& pos);
    
    // Calculate field strength at a point
    glm::vec3 calculateFieldStrength(const glm::vec3& pos);
    
    // Generate field line from starting point
    void traceFieldLine(const glm::vec3& startDir, float startRadius);
    
    // Generate contour at specific radius
    void generateContour(float radius, const glm::vec3& color, float alpha);
};

#endif //GRAVITATIONALFIELD_H