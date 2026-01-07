#ifndef STARFIELD_H
#define STARFIELD_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <random>

class Starfield
{
public:
    // Star properties
    struct Star {
        glm::vec3 position;
        float brightness;
        float size;
        glm::vec3 color;
    };
    
    // Rendering data
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    std::vector<Star> stars;
    
    // Constructor
    Starfield(unsigned int numStars = 1000, float radius = 50.0f);
    
    // Destructor
    ~Starfield();
    
    // Initialize geometry
    void setupMesh();
    
    // Generate random starfield
    void generateStars(unsigned int numStars, float radius);
    
    // Get rendering data
    unsigned int getVAO() const { return VAO; }
    unsigned int getStarCount() const { return stars.size(); }
    
private:
    std::mt19937 rng;
};

#endif //STARFIELD_H