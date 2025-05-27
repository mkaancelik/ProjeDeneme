// main.cpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cmath> // For M_PI, cos, sin if not using GLM for everything

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h" // Our simplified mesh
#include "CubeVertices.h"


// ImGui
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


// Screen dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f)); // Initial camera position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseCaptured = true; // Start with mouse captured for camera control

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Museum Objects
struct MuseumObject {
    std::string name;
    std::string description;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color;
    Mesh* mesh; // Each object will use a mesh (e.g., a cube for now)
    bool scanned;
};
std::vector<MuseumObject> museumObjects;
int currentScannedObjectIndex = -1; // -1 means no object info displayed

// Robot
struct Robot {
    glm::vec3 position;
    glm::vec3 initialPosition;
    float orientation; // Angle in Y axis
    Mesh* bodyMesh;
    Mesh* armMesh; // Simplified arm
    float armAngle; // For simple animation
    int currentTargetObjectIndex;
    bool autoMode;
    bool returningHome;
    float moveSpeed;
};
Robot robot;

// Lighting
glm::vec3 mainLightPos(0.0f, 10.0f, 0.0f);
glm::vec3 mainLightColor(1.0f, 1.0f, 1.0f);

// Spotlight properties
glm::vec3 spotLightPos = glm::vec3(0.0f, 5.0f, 0.0f); // Will be updated by robot
glm::vec3 spotLightDir = glm::vec3(0.0f, -1.0f, 0.0f); // Points straight down
bool spotLightOn = false;


// Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


void initMuseumObjects(Mesh* cubeMesh) {
    museumObjects.push_back({ "Statue of Hercules", "A famous Roman copy of a Greek original.", glm::vec3(-4.0f, 0.5f, -4.0f), glm::vec3(1.0f), glm::vec3(0.7f, 0.7f, 0.7f), cubeMesh, false });
    museumObjects.push_back({ "Ancient Vase", "A well-preserved vase from 500 BC.", glm::vec3(4.0f, 0.5f, -4.0f), glm::vec3(0.5f, 1.0f, 0.5f), glm::vec3(0.8f, 0.5f, 0.2f), cubeMesh, false });
    museumObjects.push_back({ "Sarcophagus Lid", "Detailed carvings depict scenes of mythology.", glm::vec3(-4.0f, 0.25f, 4.0f), glm::vec3(2.0f, 0.5f, 1.0f), glm::vec3(0.6f, 0.6f, 0.5f), cubeMesh, false });
    museumObjects.push_back({ "Mosaic Panel", "A colorful mosaic showing daily life.", glm::vec3(4.0f, 1.0f, 4.0f), glm::vec3(1.5f, 1.5f, 0.2f), glm::vec3(0.5f, 0.7f, 0.8f), cubeMesh, false });
    museumObjects.push_back({ "Gold Coin Hoard", "A collection of rare gold coins.", glm::vec3(0.0f, 0.25f, -6.0f), glm::vec3(0.5f), glm::vec3(0.9f, 0.8f, 0.2f), cubeMesh, false });
}

void initRobot(Mesh* bodyMesh, Mesh* armMesh) {
    robot.initialPosition = glm::vec3(0.0f, 0.25f, 6.0f);
    robot.position = robot.initialPosition;
    robot.orientation = glm::radians(180.0f); // Facing towards -Z initially
    robot.bodyMesh = bodyMesh;
    robot.armMesh = armMesh;
    robot.armAngle = 0.0f;
    robot.currentTargetObjectIndex = -1; // -1 for no target, 0-4 for objects
    robot.autoMode = false;
    robot.returningHome = false;
    robot.moveSpeed = 2.0f;
}

// Function to make robot move towards a target
void moveRobot(float dt) {
    if (robot.currentTargetObjectIndex < 0 && !robot.returningHome) return;

    glm::vec3 targetPos;
    if (robot.returningHome) {
        targetPos = robot.initialPosition;
    }
    else if (robot.currentTargetObjectIndex >= 0 && robot.currentTargetObjectIndex < museumObjects.size()) {
        // Target a position slightly in front of the object
        targetPos = museumObjects[robot.currentTargetObjectIndex].position - glm::vec3(0.0f, 0.0f, 1.5f); // Adjust offset as needed
        targetPos.y = robot.position.y; // Keep robot on the ground plane
    }
    else {
        return; // Invalid target
    }

    glm::vec3 directionToTarget = glm::normalize(targetPos - robot.position);
    float distanceToTarget = glm::distance(robot.position, targetPos);

    if (distanceToTarget > 0.1f) { // Threshold to stop
        // Orient robot towards target
        robot.orientation = atan2(directionToTarget.x, directionToTarget.z);
        robot.position += directionToTarget * robot.moveSpeed * dt;
    }
    else {
        // Reached target
        if (robot.returningHome) {
            robot.returningHome = false;
            robot.autoMode = false; // Stop auto mode when home
            std::cout << "Robot returned to initial position." << std::endl;
        }
        else {
            // "Scan" the object
            if (!museumObjects[robot.currentTargetObjectIndex].scanned) {
                std::cout << "Scanning: " << museumObjects[robot.currentTargetObjectIndex].name << std::endl;
                museumObjects[robot.currentTargetObjectIndex].scanned = true; // Mark as scanned (for UI popup perhaps)
                currentScannedObjectIndex = robot.currentTargetObjectIndex; // Show info for this object
                // Simple arm animation
                robot.armAngle = glm::radians(45.0f);
            }

            if (robot.autoMode) {
                robot.currentTargetObjectIndex++;
                if (robot.currentTargetObjectIndex >= museumObjects.size()) {
                    robot.currentTargetObjectIndex = -1; // All objects scanned
                    robot.returningHome = true; // Head home
                    std::cout << "All objects scanned. Returning home." << std::endl;
                }
            }
            else {
                robot.currentTargetObjectIndex = -1; // Stop manual targeting
            }
        }
    }
    // Reset arm angle if not actively scanning
    if (robot.armAngle > 0 && distanceToTarget > 0.2f) { // If moving away or target changed
        robot.armAngle -= glm::radians(90.0f) * dt; // Animate arm back
        if (robot.armAngle < 0.0f) robot.armAngle = 0.0f;
    }
}


void renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Main control window
    ImGui::Begin("Virtual Museum Controls");

    if (ImGui::CollapsingHeader("Lighting")) {
        ImGui::ColorEdit3("Main Light Color", glm::value_ptr(mainLightColor));
        ImGui::DragFloat3("Main Light Position", glm::value_ptr(mainLightPos), 0.1f);
        ImGui::Checkbox("Spotlight On/Off", &spotLightOn);
    }

    if (ImGui::CollapsingHeader("Robot Control")) {
        if (ImGui::Button("Start Automatic Tour (1-5 & Return)")) {
            robot.autoMode = true;
            robot.returningHome = false;
            robot.currentTargetObjectIndex = 0; // Start with the first object
            for (auto& obj : museumObjects) obj.scanned = false; // Reset scanned status
            currentScannedObjectIndex = -1;
        }
        if (ImGui::Button("Stop Robot / Return Home")) {
            robot.autoMode = false;
            robot.returningHome = true;
            robot.currentTargetObjectIndex = -1; // No specific object target
        }

        ImGui::Text("Manual Object Selection:");
        for (int i = 0; i < museumObjects.size(); ++i) {
            if (ImGui::Button(museumObjects[i].name.c_str())) {
                robot.autoMode = false;
                robot.returningHome = false;
                robot.currentTargetObjectIndex = i;
                currentScannedObjectIndex = -1; // Clear previous scan info until new scan
                museumObjects[i].scanned = false; // Allow re-scan
            }
        }
        ImGui::Text("Robot Position: (%.2f, %.2f, %.2f)", robot.position.x, robot.position.y, robot.position.z);
        ImGui::SliderFloat("Robot Arm Angle (Debug)", &robot.armAngle, 0.0f, glm::radians(90.0f));

    }
    if (ImGui::CollapsingHeader("Camera Control")) {
        ImGui::Text(mouseCaptured ? "Mouse Captured (Press M to release)" : "Mouse Released (Press M to capture)");
        ImGui::Text("Use WASDQE for movement, Mouse to look.");
    }


    ImGui::End();

    // Object Information Pop-up
    if (currentScannedObjectIndex != -1 && museumObjects[currentScannedObjectIndex].scanned) {
        ImGui::Begin("Object Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("%s", museumObjects[currentScannedObjectIndex].name.c_str());
        ImGui::Separator();
        ImGui::TextWrapped("%s", museumObjects[currentScannedObjectIndex].description.c_str());
        if (ImGui::Button("Close")) {
            currentScannedObjectIndex = -1; // Close the window
        }
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Virtual Museum Assignment", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // Capture mouse cursor
    if (mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader objectShader("shaders/basic.vert", "shaders/basic.frag");

    // Setup Mesh data (using the hardcoded cube)
    std::vector<float> cubeVertexData(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(float));
    Mesh cubeMesh(cubeVertexData);

    // Initialize museum room, objects, robot
    // Room (a large cube acting as the floor/walls)
    Mesh roomMesh(cubeVertexData); // Using cube for simplicity

    initMuseumObjects(&cubeMesh);
    initRobot(&cubeMesh, &cubeMesh); // Using cube for robot body and arm for now

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update robot movement/logic
        if (robot.autoMode || robot.returningHome || robot.currentTargetObjectIndex != -1) {
            moveRobot(deltaTime);
        }
        // Update spotlight to be on the robot's arm or front
        spotLightPos = robot.position + glm::vec3(0, 0.5f, 0); // Above robot
        float robotFrontX = sin(robot.orientation);
        float robotFrontZ = cos(robot.orientation);
        spotLightDir = glm::normalize(glm::vec3(robotFrontX, -0.5f, robotFrontZ)); // Pointing forward and slightly down


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        objectShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        objectShader.setVec3("viewPos", camera.Position);

        // Main light properties
        objectShader.setVec3("lightPos", mainLightPos);
        objectShader.setVec3("lightColor", mainLightColor);

        // Spotlight properties
        objectShader.setVec3("spotLightPos", spotLightPos);
        objectShader.setVec3("spotLightDir", spotLightDir);
        objectShader.setFloat("spotLightCutOff", glm::cos(glm::radians(12.5f)));
        objectShader.setFloat("spotLightOuterCutOff", glm::cos(glm::radians(17.5f)));
        objectShader.setVec3("spotLightColor", glm::vec3(1.0f, 1.0f, 0.8f)); // Yellowish spotlight
        objectShader.setBool("spotLightOn", spotLightOn);


        // Render the room (a large flattened cube as floor, and optionally walls)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // Floor position
        model = glm::scale(model, glm::vec3(20.0f, 0.1f, 20.0f)); // Large floor
        roomMesh.Draw(objectShader, model, glm::vec3(0.5f, 0.5f, 0.5f), mainLightPos, camera.Position, mainLightColor);
        // TODO: Add walls for the room

        // Render museum objects
        for (const auto& obj : museumObjects) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model = glm::scale(model, obj.scale);
            obj.mesh->Draw(objectShader, model, obj.color, mainLightPos, camera.Position, mainLightColor);
        }

        // Render robot
        // Body
        model = glm::mat4(1.0f);
        model = glm::translate(model, robot.position);
        model = glm::rotate(model, robot.orientation, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.8f)); // Robot body size
        robot.bodyMesh->Draw(objectShader, model, glm::vec3(0.2f, 0.2f, 0.8f), mainLightPos, camera.Position, mainLightColor);

        // Arm (simple rotating cylinder/cube on top)
        glm::mat4 armModel = glm::mat4(1.0f);
        armModel = glm::translate(armModel, robot.position + glm::vec3(0.0f, 0.3f, 0.0f)); // Position arm on top of body
        armModel = glm::rotate(armModel, robot.orientation, glm::vec3(0.0f, 1.0f, 0.0f)); // Align with body
        armModel = glm::rotate(armModel, robot.armAngle, glm::vec3(1.0f, 0.0f, 0.0f)); // Arm "scan" rotation
        armModel = glm::translate(armModel, glm::vec3(0.0f, 0.0f, 0.3f)); // Offset arm forward
        armModel = glm::scale(armModel, glm::vec3(0.1f, 0.1f, 0.6f)); // Arm size
        robot.armMesh->Draw(objectShader, armModel, glm::vec3(0.1f, 0.5f, 0.1f), mainLightPos, camera.Position, mainLightColor);


        // Render ImGui UI
        renderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (!ImGui::GetIO().WantCaptureKeyboard && mouseCaptured) { // Only process camera movement if ImGui doesn't want keyboard and mouse is captured
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        mouseCaptured = !mouseCaptured;
        if (mouseCaptured) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true; // Reset firstMouse to avoid jump
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (!mouseCaptured) return; // Only process if mouse is captured

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (!ImGui::GetIO().WantCaptureMouse) { // Only process camera if ImGui doesn't want mouse
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!ImGui::GetIO().WantCaptureMouse && mouseCaptured) { // Only process if ImGui doesn't want mouse and captured
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
