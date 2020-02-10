#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "shader_s.h"
#include "camera.h"
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


// volume texture file path
const char* volumnfile = "./data/head256_256x256x225.raw";

// volume texture size
int X = 256;
int Y = 256;
int Z = 225;

// color rate
float rates = 0.4;
float alpha_1 = 0.01;
float alpha_2 = 0.04;
float alpha_3 = 0.8;
float my_color[3] = {1.0, 1.0, 1.0};

float r_rate1 = 0.9;
float g_rate1 = 0.6;
float b_rate1 = 0.4;
float r_rate2 = 0.1;
float g_rate2 = 0.25;
float b_rate2 = 0.15;

// screen size
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

// angles in x axle, y axle, and z axle
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

// step size
float stepSize = 0.005f;

// color bar options
int options = 1;

// viewport, keyboard and mouse function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// vertex array object
unsigned int VAO;

// texture objects
GLuint colorMapObj;
GLuint backDepthObj;
GLuint volumnTexObj;
GLuint frameBuffer;

// texture functions
GLuint initColorMapTex(const char* filename);
GLuint initBackDepthMapTex();
GLuint initVolumnTex(const char* filename, GLuint width, GLuint height, GLuint depth);
void initFrameBuffer(GLuint texObj);

// render function
void render(GLenum cullFace, GLuint id);
GLuint generatTex();
bool LoadTextureFromFile(const char* filename, GLuint* out_texture);

//const char* transferfile = "/Users/kokofan/Documents/Contour_Tree/data/tff.dat";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH/2, SCR_HEIGHT/2, "raycasting", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
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

				
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    const char* glsl_version = "#version 150";
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
				

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("./shader.vs",
                     "./shader.fs");
    
    Shader rayShader("./raycasting.vs",
                     "./raycasting.fs");
    
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] =
    {
        // Back face
        0.0, 0.0, 0.0,  // Bottom-left
        0.0, 1.0, 0.0,  // top-right
        1.0, 1.0, 0.0,  // bottom-right
        1.0, 1.0, 0.0,  // top-right
        1.0, 0.0, 0.0,  // bottom-left
        0.0, 0.0, 0.0,  // top-left
        // Front face
        0.0, 0.0, 1.0,  // bottom-left
        1.0, 0.0, 1.0,  // bottom-right
        1.0, 1.0, 1.0,  // top-right
        1.0, 1.0, 1.0,  // top-right
        0.0, 1.0, 1.0,  // top-left
        0.0, 0.0, 1.0,  // bottom-left
        // Left face
        0.0, 0.0, 0.0,  // top-right
        0.0, 0.0, 1.0,  // top-left
        0.0, 1.0, 1.0,  // bottom-left
        0.0, 1.0, 1.0,  // bottom-left
        0.0, 1.0, 0.0,  // bottom-right
        0.0, 0.0, 0.0,  // top-right
        // Right face
        1.0, 1.0, 1.0,  // top-left
        1.0, 0.0, 1.0,  // bottom-right
        1.0, 0.0, 0.0,  // top-right
        1.0, 0.0, 0.0,  // bottom-right
        1.0, 1.0, 0.0,  // top-left
        1.0, 1.0, 1.0,  // bottom-left
        // Bottom face
        0.0, 0.0, 1.0,  // top-right
        0.0, 0.0, 0.0,  // top-left
        1.0, 0.0, 0.0,  // bottom-left
        1.0, 0.0, 0.0,  // bottom-left
        1.0, 0.0, 1.0,  // bottom-right
        0.0, 0.0, 1.0,  // top-right
        // Top face
        0.0, 1.0, 0.0,  // top-left
        0.0, 1.0, 1.0,  // bottom-right
        1.0, 1.0, 1.0,  // top-right
        1.0, 1.0, 1.0,  // bottom-right
        1.0, 1.0, 0.0,  // top-left
        0.0, 1.0, 0.0   // bottom-left
    };

    // Vertex array and vertex buffer
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    
    // texture objects
    colorMapObj = generatTex();
    backDepthObj = initBackDepthMapTex();
    volumnTexObj = initVolumnTex(volumnfile, X, Y, Z);

    // frame buffer
    initFrameBuffer(backDepthObj);
				
    // color map options
    GLuint image_texture_1 = 0;
    LoadTextureFromFile("./blue_red.png", &image_texture_1);
    
    GLuint image_texture_2 = 0;
    LoadTextureFromFile("./purple_yellow.png", &image_texture_2);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
        // active self-defined frame buffer
        // after that, all the frame will be stored
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    
        // active backface shader
        ourShader.use();
        // just render back face
        render(GL_FRONT, ourShader.ID);

        // set frame buffer back to default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear shader ID
        glUseProgram(0);
								
        processInput(window);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // active ray casting shader
        rayShader.use();

        // setting uniforms
        rayShader.setVec2("ScreenSize", (float)SCR_WIDTH, (float)SCR_HEIGHT);
        rayShader.setFloat("StepSize", stepSize);

        // 1D texture (color map)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorMapObj);
        rayShader.setInt("TransferFunc", 0);

        // 2D texture (back depth map)
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, backDepthObj);
        rayShader.setInt("ExitPoints", 1);

        // 3D volumn texture
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, volumnTexObj);
        rayShader.setInt("VolumeTex", 2);

        glEnable(GL_DEPTH_TEST);
    
        // render front face
        render(GL_BACK, rayShader.ID);

        glUseProgram(0);
								
        {
            ImGui::Begin("Menu");

            ImGui::Text("Intial Color Map Options: %d", options);
            ImGui::ImageButton((void*)(intptr_t)image_texture_1, ImVec2(200, 20));
            if (ImGui::IsItemActive())
            {
                            r_rate1 = 0.9;
                            g_rate1 = 0.6;
                            b_rate1 = 0.4;
                
                            r_rate2 = 0.1;
                            g_rate2 = 0.25;
                            b_rate2 = 0.15;
                
                            colorMapObj = generatTex();
                            options = 1;
            }
            ImGui::SameLine();
            ImGui::Text("Option 1");

            ImGui::ImageButton((void*)(intptr_t)image_texture_2, ImVec2(200, 20));
            if (ImGui::IsItemActive())
            {
                            r_rate1 = 0.8;
                            g_rate1 = 0.9;
                            b_rate1 = 0.7;
                
                            r_rate2 = 0.1;
                            g_rate2 = 0.2;
                            b_rate2 = 0.4;
                
                            colorMapObj = generatTex();
                            options = 2;
            }
            ImGui::SameLine();
            ImGui::Text("Option 2");

            ImGui::Text("Parameter Options");
            if (ImGui::SliderFloat("Rate", &rates, 0.0f, 1.0f))
            {
                            colorMapObj = generatTex();
            }
            if (ImGui::SliderFloat("Alpha_1", &alpha_1, 0.0f, 0.05f))
            {
                            colorMapObj = generatTex();
            }
            if (ImGui::SliderFloat("Alpha_2", &alpha_2, 0.0f, 0.1f))
            {
                            colorMapObj = generatTex();
            }
            if (ImGui::SliderFloat("Alpha_3", &alpha_3, 0.0f, 1.0f))
            {
                            colorMapObj = generatTex();
            }

         ImGui::Text("Color Pick Up");
            if (ImGui::ColorEdit3("Color", my_color))
            {
                            colorMapObj = generatTex();
            }

            ImGui::Text("Real-time Color Map:");
            ImGui::Image((void*)(intptr_t)colorMapObj, ImVec2(200, 25));
            ImGui::End();
        }
        
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
				
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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
        angleX += 0.5f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        angleX -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        angleY += 0.5f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        angleY -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        angleZ -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        angleZ += 0.5f;
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
    float yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top
    
    
    lastX = xpos;
    lastY = ypos;
    
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


// Render
void render(GLenum cullFace, GLuint id)
{
    // basic setting
    glClearColor(0.2f,0.2f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // MVP ( projection, view, model)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, -0.5f, -0.5f));
    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
    // notice the multiplication order: reverse order of transform
    glm::mat4 mvp = projection * view * model;

    // pass MVP to shader
    GLuint mvpIdx = glGetUniformLocation(id, "MVP");
    if (mvpIdx >= 0)
    {
        glUniformMatrix4fv(mvpIdx, 1, GL_FALSE, &mvp[0][0]);
    }
    else
    {
        std::cerr << "can't get the MVP" << std::endl;
    }

    // enable face culling and draw box
    glEnable(GL_CULL_FACE);
    glCullFace(cullFace);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDisable(GL_CULL_FACE);
    glBindVertexArray(0);
}


// init the 2D texture for render backface depth map
GLuint initBackDepthMapTex()
{
    GLuint backFaceTex;
    glGenTextures(1, &backFaceTex);
    glBindTexture(GL_TEXTURE_2D, backFaceTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return backFaceTex;
}

// init the framebuffer which stores backface depth map
void initFrameBuffer(GLuint texObj)
{
    // create a depth buffer for framebuffer
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

    // attach the texture and the depth buffer to the framebuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texObj, 0);

    // Check if framebuffer is complete
    GLenum complete = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (complete != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "framebuffer is not complete" << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// init 3D texture to store the volume data used fo ray casting
GLuint initVolumnTex(const char* filename, GLuint w, GLuint h, GLuint d)
{
    //open file
    FILE *fp;
    size_t size = w * h * d;
    GLubyte *data = new GLubyte[size];
    if (!(fp = fopen(filename, "rb")))
    {
        std::cout << "Error: opening .raw file failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "OK: open .raw file successed" << std::endl;
    }

    // read file
    if ( fread(data, sizeof(GLubyte), size, fp)!= size)
    {
        std::cout << "Error: read .raw file failed" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "OK: read .raw file successed" << std::endl;
    }
    fclose(fp);

    // generate 3d texture
    glGenTextures(1, &volumnTexObj);
    glBindTexture(GL_TEXTURE_3D, volumnTexObj);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // pixel transfer happens here from client to OpenGL server
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, w, h, d, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    delete []data;
    std::cout << "volume texture created" << std::endl;
    return volumnTexObj;
}


GLuint generatTex()
{
    float pixels[1024];

    float red_rate1 = r_rate1;
    float green_rate1 = g_rate1;
    float blue_rate1 = b_rate1;

    float red_rate2 = r_rate2;
    float green_rate2 = g_rate2;
    float blue_rate2 = b_rate2;
				
				
    for(int i = 0; i < 1024; i+=4)
    {
        if (i < 1024 * rates)
        {
            if (i != 0 && i % 128 == 0)
            {
                            red_rate1 -= 0.05;
                            green_rate1 -= 0.05;
                            blue_rate1 -= 0.05;
            }
            pixels[i] = my_color[0] - red_rate1;
            pixels[i+1] = my_color[1] - green_rate1;
            pixels[i+2] = my_color[2] - blue_rate1;
        }
        else
        {
            if (i != 0 && i % 128 == 0)
            {
                            red_rate2 += 0.05;
                            green_rate2 += 0.1;
                            blue_rate2 += 0.1;
            }

            pixels[i] = my_color[0] - red_rate2;
            pixels[i+1] = my_color[1] - green_rate2;
            pixels[i+2] = my_color[2] - blue_rate2;
        }

        if (i < 160)
            pixels[i+3] = alpha_1;
        else if (i < 500)
            pixels[i+3] = alpha_2;
        else
            pixels[i+3] = alpha_3;
    }
    
    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 1, 0, GL_RGBA, GL_FLOAT, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture1;
}


bool LoadTextureFromFile(const char* filename, GLuint* out_texture)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
                    return false;
    
    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);
    
    *out_texture = image_texture;
    
    return true;
}
