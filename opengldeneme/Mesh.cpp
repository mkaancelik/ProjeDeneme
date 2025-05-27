// Mesh.cpp
#include "Mesh.h"

Mesh::Mesh(const std::vector<float>& vertexData) : vertices(vertexData) {
    setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader, const glm::mat4& modelMatrix, const glm::vec3& color,
    const glm::vec3& lightPos, const glm::vec3& viewPos, const glm::vec3& lightColor) {
    shader.use();
    shader.setMat4("model", modelMatrix);
    shader.setVec3("objectColor", color);
    shader.setVec3("lightColor", lightColor);
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", viewPos);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6); // 6 floats per vertex (pos + normal)
    glBindVertexArray(0);
}