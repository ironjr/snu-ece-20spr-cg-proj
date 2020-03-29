#define GLM_ENABLE_EXPERIMENTAL
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "geometry.h"
#include "geometry_primitives.h"
#include "math_utils.h"
#include "texture.h"
#include "texture_cube.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// setting
bool isWindowed = true;
bool isKeyboardProcessed[1024] = {0};
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
engine::Camera *currentCamera;
float lastX = SCR_WIDTH * 0.5f;
float lastY = SCR_HEIGHT * 0.5f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // GLFW: window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2019-20239 Jaerin Lee", NULL, NULL);
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

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: Load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Define normal shader and skybox shader.
    Shader shader("../shaders/shader.vs", "../shaders/shader.fs"); // you can name your shader files however you like
    Shader skyboxShader("../shaders/shader_skybox.vs","../shaders/shader_skybox.fs");

    // Define required VAOs of the geometries(textured cube, skybox, quad)
    engine::Geometry cube = engine::Geometry("../resources/Shape Primitives/cubePT.json");
    engine::Geometry skybox = engine::Geometry("../resources/Shape Primitives/skyboxP.json");

    // Define textures (container, grass, grass_ground) & cubemap textures (day, night)
    // Object textures
    engine::Texture texBox = engine::Texture("../resources/Textures/container.jpg");
    engine::Texture texGrass = engine::Texture("../resources/Textures/grass.png");
    engine::Texture texGrassGnd = engine::Texture("../resources/Textures/grass_ground.jpg");

    // Environment textures
    std::vector<std::string> cubeFaceDay =
    {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    for (auto it = cubeFaceDay.begin(); it != cubeFaceDay.end(); ++it)
    {
        std::stringstream ss;
        ss << "../resources/Textures/Sky Textures/" << *it;
        *it = ss.str();
    }
    std::vector<std::string> cubeFaceNight =
    {
        "nightRight.png",
        "nightLeft.png",
        "nightTop.png",
        "nightBottom.png",
        "nightFront.png",
        "nightBack.png"
    };
    for (auto it = cubeFaceNight.begin(); it != cubeFaceNight.end(); ++it)
    {
        std::stringstream ss;
        ss << "../resources/Textures/Night Sky Textures/" << *it;
        *it = ss.str();
    }
    engine::CubemapTexture texCubeDaySky = engine::CubemapTexture(cubeFaceDay);
    engine::CubemapTexture texCubeNightSky = engine::CubemapTexture(cubeFaceNight);

    // Set texture & skybox texture uniform value (initialization)
    shader.use();
    shader.setInt("texture", GL_TEXTURE0);

    skyboxShader.use();
    skyboxShader.setInt("cubemap1", GL_TEXTURE0);
    skyboxShader.setInt("cubemap2", GL_TEXTURE1);

    // Set default camera
    engine::Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
    currentCamera = &camera;

    // TODO this codes should go to world description codes
    // World space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  1.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  2.2f, -2.5f),
        glm::vec3(-3.8f,  2.0f, -12.3f),
        glm::vec3( 2.4f,  1.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  1.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Positions of our grass objects
    const int n_grass = 1000;
    float grassGroundSize = 20;
    glm::vec3 grassPositions[n_grass];
    for (unsigned int i = 0; i < n_grass; ++i){
        float s = grassGroundSize / 2;
        float x = getUniformRandomBetweenf(-s, s);
        float z = getUniformRandomBetweenf(-s, s);
        grassPositions[i].x = x;
        grassPositions[i].y = 0.5f;
        grassPositions[i].z = z;
    }

    // Render loop
    // -----------
    while (!glfwWindowShouldClose(window)){
        // Maintain loop time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        // Input
        // -----
        processInput(window);

        // Render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /////////////////////////////////////////////////////
        // TODO : Main rendering loop
        /////////////////////////////////////////////////////

        // (1) Render boxes(cube) using normal shader.

        // Bind VAO and textures
        glBindVertexArray(cube.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texBox.ID);

        // Get the shader ready
        shader.use();
        shader.setMat4("projection", currentCamera->getProjectionMatrix(SCR_WIDTH, SCR_HEIGHT));
        shader.setMat4("view", currentCamera->getViewMatrix());

        // Render boxes
        for (unsigned int i = 0; i < 10; i++)
        {
            // locate the cubes where you want!
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            // model = model * glm::mat4_cast(cubeRotations[i]);
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, cube.numVertices);
        }

        // (2) Render ground(quad) using normal shader.
        

        // (3) Render billboard grasses(quad) using normal shader.
        

        // (4) Render skybox using skybox shader.
        glDepthMask(GL_FALSE);

        float skyboxScale = 100.0f;
        skyboxShader.use();
        skyboxShader.setMat4("projection", currentCamera->getProjectionMatrix(SCR_WIDTH, SCR_HEIGHT));
        skyboxShader.setMat4("view", currentCamera->getViewMatrix());
        skyboxShader.setMat4("model", glm::scale(glm::vec3(skyboxScale, skyboxScale, skyboxScale)));

        glBindVertexArray(skybox.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeDaySky.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeNightSky.ID);
        glDrawArrays(GL_TRIANGLES, 0, skybox.numVertices);

        skyboxShader.setFloat("mixratio", 1.0f);
        glDepthMask(GL_TRUE);



        // rendering pseudo-code

        // update projection, view matrix to shader
        // for each model:
        //      bind VAO, texture
        //      for each entity that belongs to the model:
        //          update model(transformation) matrix to shader
        //          glDrawArrays or glDrawElements


        lastFrame = currentFrame;

        // GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: De-allocate all resources once they've outlived their purpose:


    // GLFW: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)
    // 2D translation
    // TODO: this should be in the separate player controller class
    
    int moveForward = 0;
    int moveRight = 0;
    if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
    {
        ++moveForward;
    }
    if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
    {
        --moveForward;
    }
    if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
    {
        --moveRight;
    }
    if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
    {
        ++moveRight;
    }

    if (moveForward == 1)
    {
        currentCamera->processKeyboard(
            engine::CameraMovement::FORWARD,
            deltaTime
        );
    }
    else if (moveForward == -1)
    {
        currentCamera->processKeyboard(
            engine::CameraMovement::BACKWARD,
            deltaTime
        );
    }
    if (moveRight == 1)
    {
        currentCamera->processKeyboard(
            engine::CameraMovement::RIGHT,
            deltaTime
        );
    }
    else if (moveRight == -1)
    {
        currentCamera->processKeyboard(
            engine::CameraMovement::LEFT,
            deltaTime
        );
    }

    // Dayfactor
    // if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    // {
    //     ++rotCcw;
    // }
    // if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    // {
    //     --rotCcw;
    // }
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// GLFW: Whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Calculate how much cursor have moved, rotate camera proportional to the value, using processMouseMovement.
    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    currentCamera->processMouseMovement(xoffset, yoffset);
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    currentCamera->processMouseScroll(yoffset);
}
