#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>

#include "base/system_global.h"
#include "base/engine_global.h"

#include "base/camera.h"
#include "base/scene.h"

#include "data/geometry.h"
#include "data/mesh.h"
#include "data/model.h"
#include "data/shader.h"
#include "data/texture.h"
#include "data/texture_cube.h"

#include "utils/math_utils.h"
#include "utils/screen_utils.h"

using namespace std::string_literals;


// Callbacks and interfaces
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


int main()
{
    // GLFW: Initialize and configure.
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW"s << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // Uncomment this statement to fix compilation on OS X.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW Window creation.
    GLFWwindow* window = glfwCreateWindow(
        screen.width,
        screen.height,
        screen.name.c_str(),
        nullptr,
        nullptr
    );
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window"s << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Tell GLFW to capture our mouse.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD: Load all OpenGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD"s << std::endl;
        return -1;
    }

    // Configure global OpenGL state.
    // glEnable(GL_DEPTH_TEST);

    // Create scenes and declare assets.
    // TODO replace this whole initialization to parsing some serialized database.
    engine::Scene* scene = new engine::Scene();

    // Build and compile our shader program.
    engine::Shader* rtShader = new engine::Shader(
        "Ray-Tracing Shader"s,
        "../shaders/shader_ray_tracing.vert"s,
        "../shaders/shader_ray_tracing.frag"s
    );
    scene->addShader(rtShader);

    engine::Geometry* quadGeometry = new engine::Geometry(
        "Quad"s, "../resources/shape_primitive/quadPT.json"s
    );
    scene->addGeometry(quadGeometry);

    engine::CubemapTexture* skyboxTexture = new engine::CubemapTexture(
        "Default Skybox"s, 
        std::vector<std::string> {
            "../resources/cubemap/skybox/right.jpg"s,
            "../resources/cubemap/skybox/left.jpg"s,
            "../resources/cubemap/skybox/top.jpg"s,
            "../resources/cubemap/skybox/bottom.jpg"s,
            "../resources/cubemap/skybox/front.jpg"s,
            "../resources/cubemap/skybox/back.jpg"s
        }
    );
    scene->addCubemapTexture(skyboxTexture);

    // engine::Model* icosphereModel = new engine::Model(
    //     "Icosphere"s,
    //     "../resources/prop/icosphere.obj"s
    // );
    // scene->addModel(icosphereModel);


    // unsigned int rtShaderUBI = glGetUniformBlockIndex(rtShader->ID, "mesh_vertices_ubo");
    // glUniformBlockBinding(rtShader->ID, rtShaderUBI, 0);

    // Use this code for mesh render
    // unsigned int uboMeshVertices;
    // glGenBuffers(1, &uboMeshVertices);
    // glBindBuffer(GL_UNIFORM_BUFFER, uboMeshVertices);
    // glBufferData(GL_UNIFORM_BUFFER, 4000 * sizeof(float), &icosphereModel->mesh->vertices[0], GL_STATIC_DRAW);
    // glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMeshVertices, 0, 4000 * sizeof(float));
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);


    rtShader->use();
    rtShader->setFloat("environmentMap", 0.0f);
    rtShader->setVec3("ambientLightColor", glm::vec3(0.02f));

    // Set materials. You can change this.

    // Ground plane
    rtShader->setVec3("material_ground.Ka", glm::vec3(0.3f, 0.3f, 0.1f));
    rtShader->setVec3(
        "material_ground.Kd",
        glm::vec3(194 / 255.0f * 0.6f, 186 / 255.0f * 0.6f, 151 / 255.0f * 0.6f)
    );
    rtShader->setVec3("material_ground.Ks", glm::vec3(0.4f, 0.4f, 0.4f));
    rtShader->setFloat("material_ground.shininess", 88.0f);
    rtShader->setVec3("material_ground.R0", glm::vec3(0.05f));
    rtShader->setInt("material_ground.scatter_type", 0);

    // Mirror
    rtShader->setVec3("material_mirror.Kd", glm::vec3(0.03f, 0.03f, 0.08f));
    rtShader->setVec3("material_mirror.R0", glm::vec3(1.0f));
    rtShader->setInt("material_mirror.scatter_type", 0);

    // Dielectric glass
    rtShader->setFloat("material_dielectric_glass.ior", 1.5f);
    rtShader->setVec3(
        "material_dielectric_glass.extinction_constant",
        glm::log(glm::vec3(0.80f, 0.89f, 0.75f))
    );
    rtShader->setVec3(
        "material_dielectric_glass.shadow_attenuation_constant",
        glm::vec3(0.4f, 0.7f, 0.4f)
    );
    rtShader->setInt("material_dielectric_glass.scatter_type", 2);

    // Material of the box
    rtShader->setVec3("material_box.Kd", glm::vec3(0.3f, 0.3f, 0.6f));
    rtShader->setVec3("material_box.Ks", glm::vec3(0.3f, 0.3f, 0.6f));
    rtShader->setFloat("material_box.shininess", 200.0f);
    rtShader->setVec3("material_box.R0", glm::vec3(0.1f));
    rtShader->setInt("material_box.scatter_type", 0);

    // Lambertian material
    rtShader->setVec3("material_lambert.Kd", glm::vec3(0.8f, 0.8f, 0.0f));
    rtShader->setInt("material_lambert.scatter_type", 1);

    // Gold
    rtShader->setVec3("material_gold.Kd", glm::vec3(0.8f, 0.6f, 0.2f) * 0.001f);
    rtShader->setVec3("material_gold.Ks", glm::vec3(0.4f, 0.4f, 0.2f));
    rtShader->setFloat("material_gold.shininess", 200.0f);
    rtShader->setVec3("material_gold.R0", glm::vec3(0.8f, 0.6f, 0.2f));
    rtShader->setInt("material_gold.scatter_type", 3);


    while (!glfwWindowShouldClose(window))
    {
        // Get current camera.
		engine::Camera* currentCamera = gameManager.defaultCamera;

        // Maintain loop time.
        timer.update((float)glfwGetTime());

        // Process input.
        processInput(window);
        gameManager.processCommands(&cmd);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        rtShader->use();
        rtShader->setFloat("W", (GLfloat)screen.width);
        rtShader->setFloat("H", (GLfloat)screen.height);
        rtShader->setFloat("fovY", glm::radians(currentCamera->zoom));
        rtShader->setVec3("cameraPosition", currentCamera->tf.position);
        rtShader->setMat3(
            "cameraToWorldRotMatrix",
            glm::transpose(glm::mat3(currentCamera->getViewMatrix()))
        );

        // For mesh rendering
        rtShader->setInt("meshTriangleNumber", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->ID);

        glBindVertexArray(quadGeometry->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        std::cout << std::setprecision(3) << "pos( "
            << gameManager.defaultCamera->tf.position.x << ", "
            << gameManager.defaultCamera->tf.position.y << ", "
            << gameManager.defaultCamera->tf.position.z << " );  rot( "
            << gameManager.defaultCamera->tf.rotation.x << ", "
            << gameManager.defaultCamera->tf.rotation.y << ", "
            << gameManager.defaultCamera->tf.rotation.z << " )" << std::endl;

        // GLFW: Swap buffers and poll IO events (keys pressed/released, mouse moved etc.).
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional: De-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1,&VAOcube);
    //glDeleteBuffers(1, VBOcube);
    //glDeleteVertexArrays(1, &VAOquad);
    //glDeleteBuffers(1, &VBOquad);

    // GLFW: Terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void setToggle(GLFWwindow* window, unsigned int key, bool *value)
{
    if (glfwGetKey(window, key) == GLFW_PRESS && !screen.isKeyboardDone[key])
    {
        *value = !*value;
        screen.isKeyboardDone[key] = true;
    }
    if (glfwGetKey(window, key) == GLFW_RELEASE)
    {
        screen.isKeyboardDone[key] = false;
    }
}

// Process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly.
// ------------------------------------------------------------------------
void processInput(GLFWwindow* window)
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

    // Log current position and rotation.
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
    {
        std::cout << std::setprecision(3) << "pos( "
            << gameManager.defaultCamera->tf.position.x << ", "
            << gameManager.defaultCamera->tf.position.y << ", "
            << gameManager.defaultCamera->tf.position.z << " );  rot( "
            << gameManager.defaultCamera->tf.rotation.x << ", "
            << gameManager.defaultCamera->tf.rotation.y << ", "
            << gameManager.defaultCamera->tf.rotation.z << " )" << std::endl;
    }

    // Take a screenshot.
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && screen.isKeyboardDone[GLFW_KEY_V] == false)
    {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char date_char[128];
        sprintf(
            date_char, "%d_%d_%d_%d_%d_%d.png",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec
        );
        saveImage(date_char);
        screen.isKeyboardDone[GLFW_KEY_V] = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
    {
        screen.isKeyboardDone[GLFW_KEY_V] = false;
    }

    // Toggle fullscreen ? TODO
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && screen.isKeyboardDone[GLFW_KEY_Z] == false)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if (screen.isWindowed)
        {
            glfwSetWindowMonitor(
                window, glfwGetPrimaryMonitor(), 0, 0,
                mode->width, mode->height, mode->refreshRate
            );
        }
        else
        {
            glfwSetWindowMonitor(
                window, nullptr, 0, 0,
                screen.width, screen.height, mode->refreshRate
            );
        }
        screen.isWindowed = !screen.isWindowed;
        screen.isKeyboardDone[GLFW_KEY_Z] = true;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
    {
        screen.isKeyboardDone[GLFW_KEY_Z] = false;
    }

    // Update the commands.
    cmd.moveForward = moveForward;
    cmd.moveRight = moveRight;
    cmd.moveUp = moveUp;
    cmd.sprint = sprint;
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes.
// ----------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    screen.width = width;
    screen.height = height;

    // Make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// GLFW: Whenever the mouse moves, this callback is called.
// --------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (cmd.firstMouse)
    {
        cmd.lastX = (float)xpos;
        cmd.lastY = (float)ypos;
        cmd.firstMouse = false;
    }

    // Calculate how much cursor have moved, rotate camera proportional to the
    // value, using processMouseMovement.
    float xoffset = (float)xpos - cmd.lastX;
    float yoffset = cmd.lastY - (float)ypos;
    cmd.lastX = (float)xpos;
    cmd.lastY = (float)ypos;
    gameManager.processMouseMovement(xoffset, yoffset);
}

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called.
// -----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    gameManager.processMouseScroll(xoffset, yoffset);
}
