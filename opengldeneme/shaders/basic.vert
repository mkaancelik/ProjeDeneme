#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos_World; // Output position in world space
out vec3 Normal_World;  // Output normal in world space

void main()
{
    FragPos_World = vec3(model * vec4(aPos, 1.0));
    Normal_World = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos_World, 1.0);
}