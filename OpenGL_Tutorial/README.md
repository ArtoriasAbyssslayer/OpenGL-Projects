OpenGL(Open Graphics Library) is a cross platform open-source API for rendering 2D and 3D Vector graphics by communicating with the GPU

How it works: 
* Api Specification: The Khronos Group maintains OpenGL as a specification, which defines a standard software interface for 3D graphics HW.
* Hardware Interaction: The OpenGL implementation on a system translates the functions written by a developer into commands that the GPU process for HW - accelerating rendering 

#### The Rendering Pipeline
![RenderingPipeline](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/blob/master/Resources/RenderingPipeline.png)


#### Shaders - OpenCL(GLSL)
Shaders are little programs that rest on the GPU. These programs are run for each specific section of the graphics pipeline. In a basic sense, shaders are nothing more than programs transforming inputs to outputs. Shaders are also very isolated programs in that they're not allowed to communicate with each other; the only communication they have is via their inputs and outputs. 
In the previous chapter we briefly touched the surface of shaders and how to properly use them. We will now explain shaders, and specifically the OpenGL Shading Language, in a more general fashion. 

GLSL
Shaders are written in the C-like language GLS. GLSL is tailored for use with graphics and contains useful features specifically targeted at vector and matrix manipulation. 
Shaders
Shader Types:
* Vertex Shaders: 
Manipulate coordinates in 3D space and are called once per vertex. The purpose of a vertex shader is to set up the gl_Position variable - this is a special, global, built-in GLSL variable that is used to store position of current vertex.


![ShaderInterpratetionOpenGL](https://github.com/ArtoriasAbyssslayer/OpenGL-Projects/blob/master/Resources/ShaderBytesInterpretationOpenGL.png)