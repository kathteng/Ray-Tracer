# First-Hit Ray Tracer - Academic Project

## About the Project
A ray tracer written in C++ that renders a simple scene composed of two spheres with different materials and a tetrahedron. The ray traced images are displayed using an OpenGL program provided.

## How to Compile
#### If not using Visual Studio:
Ensure your machine supports OpenGL v3.3 or above.
Run the makefile in the terminal using ‘make’.

#### Using Visual Studio:


## How to Operate Program
- Press Z to switch between Parallel Camera (default camera) and Perspective Camera.
- Press and hold down ENTER to move the camera in a circle around the objects.
- Press ESC to close the window.

## Objects and Lights in the Scene
- Camera
  - By default, the camera start position is (30, -200, 90) with a lookAt vector of (0, 1,
-0.3) to see all the objects in the scene. The z-axis is the up vector. By default,
the image size is 500x500 and the pixel resolution is 512x512. Resolution is
defined in simpleTexture.cpp while the image size is set in the Camera class in
Raytracer.cpp.

- Ambient Light
  - By default, an ambient light of intensity 0.3 is put in the scene and impacts every
object in the scene.

- Directional Light
  - By default, a directional light with direction normalize((1, 1.5, -1)) is put in the
scene. It interacts with every object to do diffuse and specular shading
depending on the object. It causes shadows.

- Plane
  - The plane is made of two triangles lying on the xy-plane. It has diffuse shading,
reflection, but no specular shading. The maximum number of recursive ray
generation is limited to three due to performance cost.
- Tetrahedron
  - An orange tetrahedron is put in the scene by default. It has diffuse shading, no
specular shading, and no reflection.
- Purple Sphere
  - A purple sphere is placed at (-50, 80, -60) with a radius of 50. It has diffuse
shading, some specular shading, and no reflection.
- Green Sphere
  - A green sphere is placed at (20, 230, -20) with a radius of 90. It has diffuse
shading, no specular shading, and no reflection.
