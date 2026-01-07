#include "LensingGrid.h"

LensingGrid::LensingGrid(const glm::vec3& bhPos, float bhRadius, int size, float spacing)
    : blackHolePos(bhPos), schwarzschildRadius(bhRadius), 
      gridSize(size), gridSpacing(spacing), VAO(0), VBO(0) {
    generateGrid();
    updateVertices();
    setupMesh();
}

LensingGrid::~LensingGrid() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

glm::vec3 LensingGrid::calculateLensing(const glm::vec3& pos) {
    glm::vec3 r = pos - blackHolePos;
    float dist = length(r);
    
    if (dist < schwarzschildRadius * 1.5f) {
        return pos; // Too close to black hole
    }
    
    // Simplified gravitational lensing formula
    float einsteinRadius = schwarzschildRadius * sqrt(4.0f * dist / schwarzschildRadius);
    float deflectionAngle = einsteinRadius / (2.0f * dist);
    
    // Apply deflection perpendicular to radial direction
    glm::vec3 radialDir = normalize(r);
    glm::vec3 deflection = radialDir * deflectionAngle * 2.0f;
    
    return pos + deflection;
}

void LensingGrid::generateGrid() {
    gridPoints.clear();
    
    // Create 2D grid in XZ plane
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            GridPoint point;
            
            // Original grid position
            float x = (i - gridSize / 2) * gridSpacing;
            float z = (j - gridSize / 2) * gridSpacing;
            point.originalPos = glm::vec3(x, 0.0f, z);
            
            // Calculate distortion strength based on distance to black hole
            float dist = length(point.originalPos - blackHolePos);
            point.distortionStrength = schwarzschildRadius / (dist * dist);
            point.distortionStrength = glm::clamp(point.distortionStrength, 0.0f, 1.0f);
            
            gridPoints.push_back(point);
        }
    }
}

void LensingGrid::updateDistortion(const glm::vec3& viewPos) {
    for (auto& point : gridPoints) {
        // Apply lensing distortion
        point.distortedPos = calculateLensing(point.originalPos);
        
        // Update distortion strength based on view position for dynamic effect
        float dist = length(point.originalPos - blackHolePos);
        point.distortionStrength = schwarzschildRadius / (dist * dist);
        point.distortionStrength = glm::clamp(point.distortionStrength, 0.0f, 1.0f);
    }
    
    updateVertices();
}

void LensingGrid::generateGridLines() {
    vertices.clear();
    
    // Horizontal lines
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize - 1; ++j) {
            int idx1 = i * gridSize + j;
            int idx2 = i * gridSize + (j + 1);
            
            if (idx1 < gridPoints.size() && idx2 < gridPoints.size()) {
                const auto& p1 = gridPoints[idx1];
                const auto& p2 = gridPoints[idx2];
                
                // Line segment 1
                vertices.push_back(p1.distortedPos.x);
                vertices.push_back(p1.distortedPos.y);
                vertices.push_back(p1.distortedPos.z);
                vertices.push_back(0.3f); // R
                vertices.push_back(0.5f); // G
                vertices.push_back(0.8f); // B
                vertices.push_back(p1.distortionStrength * 0.3f); // Alpha
                
                // Line segment 2
                vertices.push_back(p2.distortedPos.x);
                vertices.push_back(p2.distortedPos.y);
                vertices.push_back(p2.distortedPos.z);
                vertices.push_back(0.3f);
                vertices.push_back(0.5f);
                vertices.push_back(0.8f);
                vertices.push_back(p2.distortionStrength * 0.3f);
            }
        }
    }
    
    // Vertical lines
    for (int i = 0; i < gridSize - 1; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            int idx1 = i * gridSize + j;
            int idx2 = (i + 1) * gridSize + j;
            
            if (idx1 < gridPoints.size() && idx2 < gridPoints.size()) {
                const auto& p1 = gridPoints[idx1];
                const auto& p2 = gridPoints[idx2];
                
                // Line segment 1
                vertices.push_back(p1.distortedPos.x);
                vertices.push_back(p1.distortedPos.y);
                vertices.push_back(p1.distortedPos.z);
                vertices.push_back(0.3f);
                vertices.push_back(0.5f);
                vertices.push_back(0.8f);
                vertices.push_back(p1.distortionStrength * 0.3f);
                
                // Line segment 2
                vertices.push_back(p2.distortedPos.x);
                vertices.push_back(p2.distortedPos.y);
                vertices.push_back(p2.distortedPos.z);
                vertices.push_back(0.3f);
                vertices.push_back(0.5f);
                vertices.push_back(0.8f);
                vertices.push_back(p2.distortionStrength * 0.3f);
            }
        }
    }
}

void LensingGrid::updateVertices() {
    generateGridLines();
}

void LensingGrid::setupMesh() {
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