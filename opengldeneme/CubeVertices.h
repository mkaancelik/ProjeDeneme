// CubeVertices.h
// Defines vertex data for a simple cube with normals.
#ifndef CUBE_VERTICES_H
#define CUBE_VERTICES_H

#include <vector>

// Cube vertices: position (3f), normal (3f)
// Note: Normals are per-face, so vertices are duplicated for each face.
const float cubeVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-right
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // bottom-right         
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // top-right
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // top-left
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // bottom-left
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // top-left
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // top-right         
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // bottom-left     
     // Bottom face
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-right
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-left
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-left
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-left
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // bottom-right
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // top-right
     // Top face
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-left
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // bottom-right
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-right     
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // bottom-right
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // top-left
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f  // bottom-left
};

const int numCubeVertices = 36; // 6 faces * 2 triangles/face * 3 vertices/triangle

#endif // CUBE_VERTICES_H
