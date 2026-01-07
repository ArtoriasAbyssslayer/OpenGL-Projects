#ifndef ACCRETIONDISK_H
#define ACCRETIONDISK_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

class AccretionDisk
{
public:
    // Accretion disk properties
    glm::vec3 position;
    float innerRadius;
    float outerRadius;
    glm::vec3 rotationAxis;
    
    // Rendering data
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Constructor
    AccretionDisk(const glm::vec3 &pos, float innerRad, float outerRad, 
                  const glm::vec3 &axis = glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Destructor
    ~AccretionDisk();
    
    // Initialize geometry
    void setupMesh();
    
    // Generate disk mesh
    void generateDisk(float innerRadius, float outerRadius, unsigned int sectors);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    float getInnerRadius() const { return innerRadius; }
    float getOuterRadius() const { return outerRadius; }
    
    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getIndexCount() const { return indices.size(); }
    
private:
    void calculateRotation();
};

#endif //ACCRETIONDISK_H