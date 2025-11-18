#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>

// Iron thermal properties
struct IronProperties {
    static constexpr double thermalConductivity = 80.4;  // W/m·K
    static constexpr double density = 7874.0;            // kg/m³
    static constexpr double specificHeat = 449.0;        // J/kg·K
    static constexpr double thermalDiffusivity = 2.3e-5; // m²/s
    static constexpr double meltingPoint = 1538.0;       // °C
};

struct HeatSource {
    int x, y;
    double temperature;
    
    HeatSource(int x_, int y_, double temp) : x(x_), y(y_), temperature(temp) {}
};

struct EikonalNode {
    int x, y;
    double distance;
    
    EikonalNode(int x_, int y_, double dist) : x(x_), y(y_), distance(dist) {}
    
    bool operator>(const EikonalNode& other) const {
        return distance > other.distance;
    }
};

enum class SimulationMode {
    HEAT_DIFFUSION,
    EIKONAL,
    COMBINED
};

class HeatDiffusionSimulator {
private:
    // SDL components
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    
    // Simulation parameters
    int windowWidth, windowHeight;
    int meshSize;
    double cellSize;
    double timeStep;
    double currentTime;
    int iterations;
    SimulationMode mode;
    
    // Temperature data
    std::vector<std::vector<double>> temperature;
    std::vector<std::vector<double>> newTemperature;
    std::vector<std::vector<double>> eikonalDistance;
    std::vector<std::vector<double>> propagationSpeed;
    std::vector<HeatSource> heatSources;
    
    // Constants
    static constexpr double AMBIENT_TEMP = 20.0;    // °C
    static constexpr double MAX_DISPLAY_TEMP = 1000.0; // °C
    static constexpr double COOLING_RATE = 0.005;
    
    // Visualization
    std::vector<Uint32> pixelBuffer;
    
    bool isRunning = false;
    bool shouldQuit = false;

public:
    HeatDiffusionSimulator(int width = 1000, int height = 800, int meshSize_ = 50) 
        : windowWidth(width), windowHeight(height), meshSize(meshSize_), 
          timeStep(0.01), currentTime(0.0), iterations(0), 
          mode(SimulationMode::HEAT_DIFFUSION) {
        
        cellSize = std::min(windowWidth, windowHeight - 100) / static_cast<double>(meshSize);
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            throw std::runtime_error("SDL initialization failed");
        }
        
        window = SDL_CreateWindow("Heat Diffusion Simulation - Iron Mesh",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            windowWidth, windowHeight, SDL_WINDOW_SHOWN);
        
        if (!window) {
            SDL_Quit();
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Window creation failed");
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Renderer creation failed");
        }
        
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
            SDL_TEXTUREACCESS_STREAMING, meshSize, meshSize);
        
        if (!texture) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
            throw std::runtime_error("Texture creation failed");
        }
        
        initializeMesh();
        pixelBuffer.resize(meshSize * meshSize);
        
        // Add initial heat source at center
        addHeatSource(meshSize / 2, meshSize / 2, 800.0);
        
        printInstructions();
    }
    
    ~HeatDiffusionSimulator() {
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    void initializeMesh() {
        temperature.assign(meshSize, std::vector<double>(meshSize, AMBIENT_TEMP));
        newTemperature.assign(meshSize, std::vector<double>(meshSize, AMBIENT_TEMP));
        eikonalDistance.assign(meshSize, std::vector<double>(meshSize, std::numeric_limits<double>::infinity()));
        propagationSpeed.assign(meshSize, std::vector<double>(meshSize, 1.0));
        
        // Initialize propagation speed based on thermal diffusivity
        double alpha = IronProperties::thermalDiffusivity;
        for (int i = 0; i < meshSize; ++i) {
            for (int j = 0; j < meshSize; ++j) {
                // Speed varies with material properties
                propagationSpeed[i][j] = std::sqrt(alpha) * 1000.0; // Scale for visualization
            }
        }
    }
    
    void printInstructions() {
        std::cout << "\n=== Heat Diffusion Simulation - Iron Mesh ===\n";
        std::cout << "Controls:\n";
        std::cout << "  SPACE    - Start/Pause simulation\n";
        std::cout << "  R        - Reset simulation\n";
        std::cout << "  1        - Heat Diffusion mode\n";
        std::cout << "  2        - Eikonal mode\n";
        std::cout << "  3        - Combined mode\n";
        std::cout << "  +/-      - Increase/Decrease time step\n";
        std::cout << "  Mouse    - Add heat source (left click)\n";
        std::cout << "  ESC      - Exit\n\n";
        std::cout << "Iron Properties:\n";
        std::cout << "  Thermal Conductivity: " << IronProperties::thermalConductivity << " W/m·K\n";
        std::cout << "  Thermal Diffusivity:  " << IronProperties::thermalDiffusivity << " m²/s\n";
        std::cout << "  Density:              " << IronProperties::density << " kg/m³\n";
        std::cout << "  Melting Point:        " << IronProperties::meltingPoint << "°C\n\n";
    }
    
    void addHeatSource(int x, int y, double temperature) {
        if (x >= 0 && x < meshSize && y >= 0 && y < meshSize) {
            heatSources.emplace_back(x, y, temperature);
            this->temperature[y][x] = temperature;
            eikonalDistance[y][x] = 0.0;
        }
    }
    
    // Dijkstra-based Eikonal equation solver
    void solveEikonal() {
        // Reset distances
        for (int i = 0; i < meshSize; ++i) {
            for (int j = 0; j < meshSize; ++j) {
                eikonalDistance[i][j] = std::numeric_limits<double>::infinity();
            }
        }
        
        std::priority_queue<EikonalNode, std::vector<EikonalNode>, std::greater<EikonalNode>> pq;
        std::vector<std::vector<bool>> visited(meshSize, std::vector<bool>(meshSize, false));
        
        // Initialize with heat sources
        for (const auto& source : heatSources) {
            eikonalDistance[source.y][source.x] = 0.0;
            pq.emplace(source.x, source.y, 0.0);
        }
        
        // 8-connected neighbors
        const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        const double edgeLength[] = {
            std::sqrt(2), 1, std::sqrt(2), 1, 1, std::sqrt(2), 1, std::sqrt(2)
        };
        
        while (!pq.empty()) {
            EikonalNode current = pq.top();
            pq.pop();
            
            if (visited[current.y][current.x]) continue;
            visited[current.y][current.x] = true;
            
            for (int i = 0; i < 8; ++i) {
                int nx = current.x + dx[i];
                int ny = current.y + dy[i];
                
                if (nx >= 0 && nx < meshSize && ny >= 0 && ny < meshSize && !visited[ny][nx]) {
                    double speed = propagationSpeed[ny][nx];
                    double newDistance = current.distance + edgeLength[i] / speed;
                    
                    if (newDistance < eikonalDistance[ny][nx]) {
                        eikonalDistance[ny][nx] = newDistance;
                        pq.emplace(nx, ny, newDistance);
                    }
                }
            }
        }
    }
    
    void updateHeatDiffusion() {
        double alpha = IronProperties::thermalDiffusivity;
        double dx = 1.0; // Normalized grid spacing
        double r = alpha * timeStep / (dx * dx);
        
        // Stability check
        if (r > 0.25) {
            std::cout << "Warning: Time step may be too large for numerical stability (r = " 
                      << r << ")\n";
        }
        
        // Update interior points using finite differences
        for (int i = 1; i < meshSize - 1; ++i) {
            for (int j = 1; j < meshSize - 1; ++j) {
                // 2D Laplacian
                double laplacian = temperature[i-1][j] + temperature[i+1][j] +
                                 temperature[i][j-1] + temperature[i][j+1] -
                                 4.0 * temperature[i][j];
                
                newTemperature[i][j] = temperature[i][j] + r * laplacian;
            }
        }
        
        // Boundary conditions (Neumann - insulated boundaries with cooling)
        for (int i = 0; i < meshSize; ++i) {
            // Top and bottom boundaries
            newTemperature[0][i] = temperature[1][i] * (1.0 - COOLING_RATE);
            newTemperature[meshSize-1][i] = temperature[meshSize-2][i] * (1.0 - COOLING_RATE);
            
            // Left and right boundaries
            newTemperature[i][0] = temperature[i][1] * (1.0 - COOLING_RATE);
            newTemperature[i][meshSize-1] = temperature[i][meshSize-2] * (1.0 - COOLING_RATE);
        }
        
        // Maintain heat sources
        for (const auto& source : heatSources) {
            newTemperature[source.y][source.x] = source.temperature;
        }
        
        // Swap temperature arrays
        std::swap(temperature, newTemperature);
    }
    
    void updateCombined() {
        solveEikonal();
        updateHeatDiffusion();
        
        // Blend eikonal influence with heat diffusion
        const double eikonalWeight = 0.15;
        for (int i = 0; i < meshSize; ++i) {
            for (int j = 0; j < meshSize; ++j) {
                if (eikonalDistance[i][j] < std::numeric_limits<double>::infinity()) {
                    double eikonalTemp = std::max(AMBIENT_TEMP, 
                        MAX_DISPLAY_TEMP * std::exp(-eikonalDistance[i][j] * 0.08));
                    temperature[i][j] = (1.0 - eikonalWeight) * temperature[i][j] + 
                                      eikonalWeight * eikonalTemp;
                }
            }
        }
    }
    
    Uint32 temperatureToColor(double temp) {
        // Normalize temperature
        double normalized = std::max(0.0, std::min(1.0, 
            (temp - AMBIENT_TEMP) / (MAX_DISPLAY_TEMP - AMBIENT_TEMP)));
        
        Uint8 r, g, b;
        
        if (normalized < 0.25) {
            r = 0;
            g = 0;
            b = static_cast<Uint8>(128 + 127 * (normalized / 0.25));
        } else if (normalized < 0.5) {
            r = 0;
            g = static_cast<Uint8>(255 * ((normalized - 0.25) / 0.25));
            b = 255;
        } else if (normalized < 0.75) {
            r = 0;
            g = 255;
            b = static_cast<Uint8>(255 * (1.0 - (normalized - 0.5) / 0.25));
        } else if (normalized < 0.9) {
            r = static_cast<Uint8>(255 * ((normalized - 0.75) / 0.15));
            g = 255;
            b = 0;
        } else {
            r = 255;
            g = static_cast<Uint8>(255 * (1.0 - (normalized - 0.9) / 0.1));
            b = 0;
        }
        
        return (0xFF << 24) | (r << 16) | (g << 8) | b;
    }
    
    void updateVisualization() {
        for (int i = 0; i < meshSize; ++i) {
            for (int j = 0; j < meshSize; ++j) {
                double displayValue;
                
                switch (mode) {
                    case SimulationMode::EIKONAL:
                        displayValue = (eikonalDistance[i][j] < std::numeric_limits<double>::infinity()) ?
                            std::max(AMBIENT_TEMP, MAX_DISPLAY_TEMP * std::exp(-eikonalDistance[i][j] * 0.1)) :
                            AMBIENT_TEMP;
                        break;
                    default:
                        displayValue = temperature[i][j];
                        break;
                }
                
                pixelBuffer[i * meshSize + j] = temperatureToColor(displayValue);
            }
        }
        
        SDL_UpdateTexture(texture, nullptr, pixelBuffer.data(), meshSize * sizeof(Uint32));
    }
    
    void render() {
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
        
        // Render temperature field
        SDL_Rect destRect = {50, 50, static_cast<int>(meshSize * cellSize), static_cast<int>(meshSize * cellSize)};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        
        // Draw heat source indicators
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (const auto& source : heatSources) {
            SDL_Rect sourceRect = {
                50 + static_cast<int>(source.x * cellSize + 2),
                50 + static_cast<int>(source.y * cellSize + 2),
                static_cast<int>(cellSize - 4),
                static_cast<int>(cellSize - 4)
            };
            SDL_RenderDrawRect(renderer, &sourceRect);
        }
        
        // Draw info text (simple rectangles as indicators)
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
        SDL_Rect infoRect = {10, windowHeight - 80, windowWidth - 20, 70};
        SDL_RenderFillRect(renderer, &infoRect);
        
        SDL_RenderPresent(renderer);
    }
    
    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    shouldQuit = true;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            shouldQuit = true;
                            break;
                            
                        case SDLK_SPACE:
                            isRunning = !isRunning;
                            std::cout << (isRunning ? "Simulation started\n" : "Simulation paused\n");
                            break;
                            
                        case SDLK_r:
                            reset();
                            std::cout << "Simulation reset\n";
                            break;
                            
                        case SDLK_1:
                            mode = SimulationMode::HEAT_DIFFUSION;
                            std::cout << "Mode: Heat Diffusion\n";
                            break;
                            
                        case SDLK_2:
                            mode = SimulationMode::EIKONAL;
                            std::cout << "Mode: Eikonal Propagation\n";
                            break;
                            
                        case SDLK_3:
                            mode = SimulationMode::COMBINED;
                            std::cout << "Mode: Combined\n";
                            break;
                            
                        case SDLK_PLUS:
                        case SDLK_EQUALS:
                            timeStep = std::min(0.1, timeStep * 1.1);
                            std::cout << "Time step: " << timeStep << "\n";
                            break;
                            
                        case SDLK_MINUS:
                            timeStep = std::max(0.001, timeStep * 0.9);
                            std::cout << "Time step: " << timeStep << "\n";
                            break;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        int x = static_cast<int>((event.button.x - 50) / cellSize);
                        int y = static_cast<int>((event.button.y - 50) / cellSize);
                        if (x >= 0 && x < meshSize && y >= 0 && y < meshSize) {
                            addHeatSource(x, y, 900.0);
                            std::cout << "Heat source added at (" << x << ", " << y << ")\n";
                        }
                    }
                    break;
            }
        }
    }
    
    void update() {
        if (!isRunning) return;
        
        switch (mode) {
            case SimulationMode::HEAT_DIFFUSION:
                updateHeatDiffusion();
                break;
            case SimulationMode::EIKONAL:
                solveEikonal();
                break;
            case SimulationMode::COMBINED:
                updateCombined();
                break;
        }
        
        currentTime += timeStep;
        iterations++;
        
        // Print status every 100 iterations
        if (iterations % 100 == 0) {
            double maxTemp = AMBIENT_TEMP;
            for (const auto& row : temperature) {
                for (double temp : row) {
                    maxTemp = std::max(maxTemp, temp);
                }
            }
            
            std::cout << std::fixed << std::setprecision(2)
                      << "Time: " << currentTime << "s, "
                      << "Max Temp: " << maxTemp << "°C, "
                      << "Iterations: " << iterations << "\n";
        }
    }
    
    void reset() {
        isRunning = false;
        currentTime = 0.0;
        iterations = 0;
        heatSources.clear();
        initializeMesh();
        addHeatSource(meshSize / 2, meshSize / 2, 800.0);
    }
    
    void run() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        
        while (!shouldQuit) {
            auto currentTimePoint = std::chrono::high_resolution_clock::now();
            auto deltaTime = std::chrono::duration<double>(currentTimePoint - lastTime).count();
            
            handleEvents();
            update();
            updateVisualization();
            render();
            
            lastTime = currentTimePoint;
            
            // Cap frame rate to ~60 FPS
            SDL_Delay(16);
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        int meshSize = 50;
        if (argc > 1) {
            meshSize = std::max(20, std::min(200, std::atoi(argv[1])));
        }
        
        HeatDiffusionSimulator simulator(1000, 800, meshSize);
        simulator.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}