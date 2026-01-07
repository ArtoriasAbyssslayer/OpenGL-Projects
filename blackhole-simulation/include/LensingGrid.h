#ifndef LENSINGGRID_H
#define LENSINGGRID_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

class LensingGrid
{
public:
    // Grid properties
    struct GridPoint {
        glm::vec3 originalPos;
        glm::vec3 distortedPos;
        float distortionStrength;
    };
    
    // Rendering data
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    std::vector<GridPoint> gridPoints;
    
    // Grid properties
    glm::vec3 blackHolePos;
    float schwarzschildRadius;
    int gridSize;
    float gridSpacing;
    
    // Constructor
    LensingGrid(const glm::vec3& bhPos, float bhRadius, 
                int size = 20, float spacing = 1.0f);
    
    // Destructor
    ~LensingGrid();
    
    // Update grid distortion
    void updateDistortion(const glm::vec3& viewPos);
    
    // Generate grid mesh
    void generateGrid();
    
    // Update rendering data
    void updateVertices();
    
    // Initialize geometry
    void setupMesh();
    
    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getVertexCount() const { return vertices.size() / 6; } // 6 floats per vertex (pos + color + alpha)
    
private:
    // Calculate gravitational lensing distortion
    glm::vec3 calculateLensing(const glm::vec3& pos);
    
    // Generate grid lines
    void generateGridLines();
};

#endif //LENSINGGRID_H