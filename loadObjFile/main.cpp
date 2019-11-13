//
//  main.cpp
//  openGL
//
//  Created by kokofan on 10/24/19.
//  Copyright © 2019 koko Fan. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>

#include "shader.h"
#include "camera.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastframe = 0.0f;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

glm::vec3 lightPos(0.6f, 0.0f, 2.0f);


void loadObj(std::string filename,  std::vector<glm::vec3> &positions, std::vector<glm::vec3> &norms)
{
    
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_norms;
    
    std::ifstream infile(filename);
    
    if(!infile)
    {
        std::cerr << "Cannot open the File: " << filename << std::endl;
    }
    
    std::string str;
    
    while (std::getline(infile, str))
    {
        std::istringstream ss(str);
        
        if(ss.peek() == 'v')
        {
            ss.ignore();
            
            if(ss.peek() == 'n')
            {
                ss.ignore();
                glm::vec3 norm;
                ss >> norm.x >> norm.y >> norm.z;
                temp_norms.push_back(norm);
            }
            else
            {
                glm::vec3 vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                temp_positions.push_back(vertex);
            }
        }
        
        else if(ss.peek() == 'f')
        {
            float x, y, z;
            ss.ignore();
            ss >> x >> y >> z;
            
            positions.push_back(temp_positions[x - 1]);
            positions.push_back(temp_positions[y - 1]);
            positions.push_back(temp_positions[z - 1]);
            
            norms.push_back(temp_norms[x - 1]);
            norms.push_back(temp_norms[y - 1]);
            norms.push_back(temp_norms[z - 1]);
        }
    }
}

// Main function
int main()
{
    
    std::string filename = "./cube.obj";
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> norms;
    loadObj(filename, positions, norms);
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Initialize and configuration, tell GLFW openGL 3.3 will be used */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Fix compilation on MAC OS */
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Load 3D object", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /* glew: load all OpenGL function pointers */
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
        return -1;
    }

    /* Print out the current version of OpenGL */
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("./shader.vs",
                     "./shader.fs"); // read
    
    Shader lampShader("./lightShader.vs",
                      "./lightShader.fs");
    
    /* Vertex buffer object, it will store a lot of vertex in GPU memory,
     and send them to graphic card*/
    /* VAO(Vertex Array Object), VBO(Vertex Buffer Object),
     EBO(Element Buffer Object)*/
    unsigned int VBO, VAO;
    // Generate a buffer and vertex object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the buffer to GL_ARRAY_BUFFER, and Vertex
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0], GL_STATIC_DRAW);

    // Copy vertexes data to buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    // The buffer of normal vector
    unsigned int normVBO;
    glGenBuffers(1, &normVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normVBO);
    glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(glm::vec3), &norms[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastframe;
        lastframe = currentFrame;

        processInput(window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Active this shader program */
        //ourShader.setFloat("xOffset", 0.3f);
        ourShader.use();
   
        ourShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("viewPos", camera.Position);
        
        ourShader.setVec3("material.ambient", 0.49225f, 0.49225f, 0.49225f);
        ourShader.setVec3("material.diffuse", 0.50754f, 0.50754f, 0.50754f);
        ourShader.setVec3("material.specular", 0.508273f, 0.508273f, 0.508273f);
        ourShader.setFloat("material.shininess", 32.0f);
        
        ourShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f); // note that all light colors are set at full intensity
        ourShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        
        //projectiom
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        
        // model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        ourShader.setMat4("model", model);

        // view
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (int)positions.size());

        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        angleX += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        angleX -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        angleY += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        angleY -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        angleZ -= 0.1f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        angleZ += 0.1f;
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


