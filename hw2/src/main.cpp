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

#include "system_global.h"
#include "engine_global.h"

#include "camera.h"
#include "geometry.h"
#include "geometry_primitives.h"
#include "interface.h"
#include "math_utils.h"
#include "shader.h"
#include "texture.h"
#include "texture_cube.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


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
    GLFWwindow* window = glfwCreateWindow(
        screen.width,
        screen.height,
        screen.name.c_str(),
        NULL,
        NULL
    );
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
    Shader skyboxShader("../shaders/shader_skybox.vs", "../shaders/shader_skybox.fs");

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
    shader.setInt("texture", 0);
    skyboxShader.use();
    skyboxShader.setInt("cubemap1", 0);
    skyboxShader.setInt("cubemap2", 1);

    // Define required VAOs of the geometries(textured cube, skybox, quad)
    engine::Geometry cube = engine::Geometry("../resources/Shape Primitives/cubePT.json");
    engine::Geometry skybox = engine::Geometry("../resources/Shape Primitives/skyboxP.json");
    engine::Geometry grass = engine::Geometry("../resources/Shape Primitives/quadPT.json");
    engine::Geometry grassGround = engine::Geometry("../resources/Shape Primitives/quadPT.json");

    // TODO this codes should go to world description codes
    float skyboxScale = 50.0f;

    // World space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f,  2.2f, -2.5f),
        glm::vec3(-3.8f,  2.0f, -12.3f),
        glm::vec3(2.4f,  1.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f,  2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  1.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    glm::quat cubeRotations[] = {
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion(),
        getRandomQuaternion()
    };

    // Positions of our grass objects
    const int n_grass = 1000;
    float grassGroundSize = 20.0f;
    glm::vec3 grassGroundPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 grassPositions[n_grass];
    for (size_t i = 0; i < n_grass; ++i){
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
        // Get current camera
		engine::Camera *currentCamera = gameManager.defaultCamera;

        // Maintain loop time
        timer.update((float)glfwGetTime());

        // Input
        // -----
        processInput(window);
        gameManager.processCommands(&cmd);

        // Render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /////////////////////////////////////////////////////
        // TODO : Main rendering loop
        /////////////////////////////////////////////////////

        // (1) Render boxes(cube) using normal shader.
        glBindVertexArray(cube.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texBox.ID);
        shader.use();
        shader.setMat4("projection", currentCamera->getProjectionMatrix(screen.width, screen.height));
        shader.setMat4("view", currentCamera->getViewMatrix());
        for (size_t i = 0; i < 10; i++)
        {
            // locate the cubes where you want!
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = model * glm::mat4_cast(cubeRotations[i]);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, cube.numVertices);
        }

        // (2) Render ground(quad) using normal shader.
        glBindVertexArray(grassGround.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texGrassGnd.ID);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, grassGroundPosition);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(grassGroundSize, grassGroundSize, 1.0f));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, grassGround.numVertices, GL_UNSIGNED_INT, 0);

        // (3) Render billboard grasses(quad) using normal shader.
		glBindVertexArray(grass.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texGrass.ID);
		for (size_t i = 0; i < n_grass; ++i){
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, grassPositions[i]);
            glm::vec3 deltaPos = currentCamera->position - grassPositions[i];
            float deltaAngle = glm::degrees(glm::atan(deltaPos.x, deltaPos.z));
            model = glm::rotate(
                model,
                glm::radians(deltaAngle), // yaw
                glm::vec3(0.0f, 1.0f, 0.0f)
			);
			shader.setMat4("model", model);
			glDrawElements(GL_TRIANGLES, grass.numVertices, GL_UNSIGNED_INT, 0);
		}

        // (4) Render skybox using skybox shader.
        glDepthMask(GL_FALSE);

        glBindVertexArray(skybox.VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeDaySky.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeNightSky.ID);
        skyboxShader.use();
        skyboxShader.setMat4("projection", currentCamera->getProjectionMatrix(screen.width, screen.height));
        skyboxShader.setMat4("view", currentCamera->getViewMatrix());
        skyboxShader.setMat4("model", glm::scale(glm::vec3(skyboxScale, skyboxScale, skyboxScale)));
        skyboxShader.setFloat("mixratio", 1.0f - gameManager.envManager->dayRatio);
        glDrawArrays(GL_TRIANGLES, 0, skybox.numVertices);

        glDepthMask(GL_TRUE);

        cmd.reset();

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

    // Make camera move by 3D translation (WASD, arrows, ascention using space).
    int moveForward = 0;
    int moveRight = 0;
    int moveUp = 0;
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        ++moveUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        --moveUp;
    }
    
    // Sprint with shift key.
    int sprint = 0;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        ++sprint;
    }

    // Increase or decrease dayFactor (O: increase, P: decrease).
    int moreDay = 0;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        ++moreDay;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        --moreDay;
    }

    // Camera interpolation with Quaternions.
    bool saveCurrentTransform = false;
    bool returnToCheckpointEuler = false;
    bool returnToCheckpointQuat = false;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        saveCurrentTransform = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        returnToCheckpointEuler = true;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        returnToCheckpointQuat = true;
    }

    // Update the commands.
    cmd.moveForward = moveForward;
    cmd.moveRight = moveRight;
    cmd.moveUp = moveUp;
    cmd.sprint = sprint;
    cmd.moreDay = moreDay;

    cmd.saveCurrentTransform = saveCurrentTransform;
    cmd.returnToCheckpointEuler = returnToCheckpointEuler;
    cmd.returnToCheckpointQuat = returnToCheckpointQuat;
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions; note that
    // width and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

// GLFW: Whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Calculate how much cursor have moved, rotate camera proportional to the
    // value, using processMouseMovement.
    float xoffset = (float)xpos - cmd.lastX;
    float yoffset = cmd.lastY - (float)ypos;
    cmd.lastX = (float)xpos;
    cmd.lastY = (float)ypos;
    gameManager.defaultCamera->processMouseMovement(xoffset, yoffset);
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    gameManager.defaultCamera->processMouseScroll((float)yoffset);
}
