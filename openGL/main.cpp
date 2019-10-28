//
//  main.cpp
//  openGL
//
//  Created by kokofan on 10/24/19.
//  Copyright © 2019 koko Fan. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.h"


/* Callback function, it will be called when the size of window changed */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Viewpoint, used to set the dimension of window
    glViewport(0, 0, width, height);
}

/* Check if user press the esc button, if it is, close the window */
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Main function
int main(void)
{
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
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    /* glew: load all OpenGL function pointers */
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
        return -1;
    }
    
    /* Print out the current version of OpenGL */
    std::cout << glGetString(GL_VERSION) << std::endl;
    
    Shader ourShader("/Users/kokofan/Documents/Contour_Tree/openGL/openGL/shader.vs", "/Users/kokofan/Documents/Contour_Tree/openGL/openGL/shader.fs"); // read
    
    /* The positions and colors of vertexes */
    float positions[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // left
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // right
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top
    };
    
    /* Vertex buffer object, it will store a lot of vertex in GPU memory,
     and send them to graphic card*/
    /* VAO(Vertex Array Object), VBO(Vertex Buffer Object),
     EBO(Element Buffer Object)*/
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the buffer to GL_ARRAY_BUFFER, and Vertex
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    // Copy vertexes data to buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    
    // Copy vertex color date to buffer
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        /* Active this shader program */
        ourShader.use();
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
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


