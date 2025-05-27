#pragma once
// Mesh.h
// Simplified Mesh class for rendering basic primitives (e.g., a cube).
// For actual model loading, you'd use Assimp and a more complex structure.
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

class Mesh {
public:
    // Mesh Data
    std::vector<float> vertices; // x, y, z, nx, ny, nz
    unsigned int VAO, VBO;

    Mesh(const std::vector<float>& vertexData);
    ~Mesh();
    void Draw(Shader& shader, const glm::mat4& model, const glm::vec3& color,
        const glm::vec3& lightPos, const glm::vec3& viewPos, const glm::vec3& lightColor);

private:
    void setupMesh();
};

#endif