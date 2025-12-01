# OpenGL-Projects
This repository is a way to explore computer graphics on low level APIs like OpenGL and Vulkan and also create some simulations that I like. 

![OpenglThumb](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/blob/master/Resources/opengl.png)

##### Listed Projects 
* [OpenGL Tutorial](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/tree/master/OpenGL_Tutorial)<br>
![Tutorial Tumbnail](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/blob/master/Resources/Tutorial_Screenshot.png) <br>

* [Black Hole Simulation]()
* [Gravity Spacetime Fabric Simulated]()
* [Oil-Spil Fluid Simulation]()
* [Fluid Simulation](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/tree/master/fluid-simulation)
* [Mesh - OpenGL]()
* [Heat Diffuse Simulation](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/tree/master/heat-diffuse)


### Installation/Build
In order to build the project CmakeLists.txt recipes are provided and you should use cmake project builder.
```sh
    cd [ProjectFolder]
    mkdir build && build 
    cmake .. -A x64 
    #(Use -G MinGW or MSVC or Ninja or whatever build tool you want)
    cmake --build --config Release # [Debug/Release]
```
