#ifndef BLACKHOLE_H
#define BLACKHOLE_H

#include <glm/glm.hpp>
#include <vector>

class BlackHole
{
public:
    // Black Hole properties
    glm::vec3 position;
    float mass;
    float schwarzschildRadius; // Event horizon radius
    float photonSphereRadius;  // Photo sphere (1.5x even horizon)

    // Rendering data
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Constructor
    BlackHole(const glm::vec3 &pos, float m);

    // Destructor
    ~BlackHole();

    // Initialize Geometry
    void setupMesh();

    // Generate Sphere Mesh
    void generateSphere(float radius, unsigned int rings, unsigned int sectors);

    // Getters
    glm::vec3 getPosition() const { return position; }
    float getMass() const { return mass; }
    float getSchwarzschildRadius() const { return schwarzschildRadius; }
    float getPhotonSphereRadius() const { return photonSphereRadius; }

    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getIndexCount() const { return indices.size(); }
private: 
    void calculateRadii();
};

#endif //BLACKHOLE_H