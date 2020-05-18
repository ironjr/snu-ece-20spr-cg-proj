#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <utility>

#include "base/system_global.h"
#include "base/engine_global.h"

#include "base/camera.h"
#include "base/scene.h"

#include "data/geometry.h"
#include "data/light.h"
#include "data/mesh.h"
#include "data/model.h"
#include "data/shader.h"
#include "data/texture.h"
#include "data/texture_cube.h"

#include "utils/math_utils.h"

using namespace std::string_literals;


// Callbacks and interfaces
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, engine::DirectionalLight* sun);


int main()
{
    // GLFW: Initialize and configure.
    glfwInit();
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
        NULL,
        NULL
    );
    if (window == NULL)
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
    glEnable(GL_DEPTH_TEST);


    // Create scenes and declare assets.
    // TODO replace this whole initialization to parsing some serialized database.
    engine::Scene* scene = new engine::Scene();

    // Build and compile the shader programs.
    engine::Shader* lightingShader = new engine::Shader(
        "Lighting Shader"s,
        "../shaders/shader_lighting.vert"s,
        "../shaders/shader_lighting.frag"s
    );
    engine::Shader* shadowShader = new engine::Shader(
        "Shadow Shader"s,
        "../shaders/shadow.vert"s,
        "../shaders/shadow.frag"s
    );
    engine::Shader* skyboxShader = new engine::Shader(
        "Skybox Shader"s,
        "../shaders/shader_skybox.vert"s,
        "../shaders/shader_skybox.frag"s
    );
    // engine::Shader* shadowDebugShader = new engine::Shader(
    //     "Shadow Debug Shader"s,
    //     "../shaders/shadow_debug.vert"s,
    //     "../shaders/shadow_debug.frag"s
    // );
    scene->addShaders(std::vector<engine::Shader*> {
        lightingShader, shadowShader, skyboxShader // , shadowDebugShader
    });

    // Define models.
    // There can be three types 
    // (1) diffuse, specular, normal : brickCubeModel
    // (2) diffuse, normal only : boulderModel
    // (3) diffuse only : grassGroundModel
    engine::Texture* diffuseTexture = nullptr;
    engine::Texture* normalTexture = nullptr;
    engine::Texture* specularTexture = nullptr;

    diffuseTexture = new engine::Texture(
        "Brick Cube Diffuse"s,
        "../resources/prop/brickcube/brickcube_d.png"s
    );
    normalTexture = new engine::Texture(
        "Brick Cube Normal"s,
        "../resources/prop/brickcube/brickcube_n.png"s
    );
    specularTexture = new engine::Texture(
        "Brick Cube Specular"s,
        "../resources/prop/brickcube/brickcube_s.png"s
    );
    scene->addTextures(std::vector<engine::Texture*> {
        diffuseTexture, normalTexture, specularTexture
    });
    engine::Model* brickCubeModel = new engine::Model(
        "Brick Cube"s, "../resources/prop/brickcube/brickcube.obj"s,
        diffuseTexture, normalTexture, specularTexture
    );
    scene->addModel(brickCubeModel);

    diffuseTexture = new engine::Texture(
        "Boulder Diffuse"s,
        "../resources/prop/boulder/boulder_d.png"s
    );
    normalTexture = new engine::Texture(
        "Boulder Normal"s,
        "../resources/prop/boulder/boulder_n.png"s
    );
    scene->addTextures(std::vector<engine::Texture*> {
        diffuseTexture, normalTexture
    });
    engine::Model* boulderModel = new engine::Model(
        "Boulder"s, "../resources/prop/boulder/boulder.obj"s,
        diffuseTexture, normalTexture, nullptr
    );
    scene->addModel(boulderModel);

    diffuseTexture = new engine::Texture(
        "Grass Ground Diffuse"s,
        "../resources/landscape/simple_ground/grass_ground.jpg"s
    );
    scene->addTexture(diffuseTexture);
    engine::Model* grassGroundModel = new engine::Model(
        "Grass Ground"s, "../resources/landscape/simple_ground/plane.obj"s,
        diffuseTexture, nullptr, nullptr //, true
    );
    scene->addModel(grassGroundModel);

    // TODO: Add more models (barrels, fire extinguisher) and YOUR own model.
    diffuseTexture = new engine::Texture(
        "Barrel Diffuse"s,
        "../resources/prop/barrel/barrel_d.png"s
    );
    normalTexture = new engine::Texture(
        "Barrel Normal"s,
        "../resources/prop/barrel/barrel_n.png"s
    );
    specularTexture = new engine::Texture(
        "Barrel Specular"s,
        "../resources/prop/barrel/barrel_s.png"s
    );
    scene->addTextures(std::vector<engine::Texture*> {
        diffuseTexture, normalTexture, specularTexture
    });
    engine::Model* barrelModel = new engine::Model(
        "Barrel"s, "../resources/prop/barrel/barrel.obj"s,
        diffuseTexture, normalTexture, specularTexture
    );
    scene->addModel(barrelModel);

    diffuseTexture = new engine::Texture(
        "Fire Extinguisher Diffuse"s,
        "../resources/prop/fireext/fireext_d.jpg"s
    );
    normalTexture = new engine::Texture(
        "Fire Extinguisher Normal"s,
        "../resources/prop/fireext/fireext_n.jpg"s
    );
    specularTexture = new engine::Texture(
        "Fire Extinguisher Specular"s,
        "../resources/prop/fireext/fireext_s.jpg"s
    );
    scene->addTextures(std::vector<engine::Texture*> {
        diffuseTexture, normalTexture, specularTexture
    });
    engine::Model* fireExtModel = new engine::Model(
        "Fire Extinguisher"s, "../resources/prop/fireext/fireext.obj"s,
        diffuseTexture, normalTexture, specularTexture
    );
    scene->addModel(fireExtModel);

    // TODO Model yourOwnModel;
    diffuseTexture = new engine::Texture(
        "Red Apple Diffuse"s,
        "../resources/prop/red_apple/red_apple_d.jpg"s
    );
    normalTexture = new engine::Texture(
        "Red Apple Normal"s,
        "../resources/prop/red_apple/red_apple_n.jpg"s
    );
    specularTexture = new engine::Texture(
        "Red Apple Specular"s,
        "../resources/prop/red_apple/red_apple_s.jpg"s
    );
    scene->addTextures(std::vector<engine::Texture*> {
        diffuseTexture, normalTexture, specularTexture
    });
    engine::Model* redAppleModel = new engine::Model(
        "Red Apple"s, "../resources/prop/red_apple/red_apple.obj"s,
        diffuseTexture, normalTexture, specularTexture
    );
    scene->addModel(redAppleModel);


    // Add entities to scene.
    // You can change the position/orientation.
    scene->addEntity(new engine::Entity(
        "Brick Cube 1"s,
        nullptr,
        engine::Transform(),
        new engine::ModelAttribute(brickCubeModel)
    ));
    scene->addEntity(new engine::Entity(
        "Brick Cube 2"s,
        nullptr,
        engine::Transform(
            glm::vec3(-3.5f, 0.0f, -2.0f),
            glm::angleAxis(glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f))
        ),
        new engine::ModelAttribute(brickCubeModel)
    ));
    scene->addEntity(new engine::Entity(
        "Brick Cube 3"s,
        nullptr,
        engine::Transform(
            glm::vec3(1.0f, 0.5f, -3.0f),
            glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        ),
        new engine::ModelAttribute(brickCubeModel)
    ));
    scene->addEntity(new engine::Entity(
        "Barrel 1"s,
        nullptr,
        engine::Transform(
            glm::vec3(2.5f, 0.0f, -2.0f),
            glm::vec3(0.0f),
            0.1f
        ),
        new engine::ModelAttribute(barrelModel)
    ));
    scene->addEntity(new engine::Entity(
        "Fire Extinguisher 1"s,
        nullptr,
        engine::Transform(
            glm::vec3(2.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 180.0f, 0.0f),
            0.002f
        ),
        new engine::ModelAttribute(fireExtModel)
    ));
    scene->addEntity(new engine::Entity(
        "Boulder 1"s,
        nullptr,
        engine::Transform(
            glm::vec3(-5.0f, 0.0f, 2.0f),
            glm::vec3(0.0f, 180.0f, 0.0f),
            0.1f
        ),
        new engine::ModelAttribute(boulderModel)
    ));

    float planeSize = 15.0f;
    scene->addEntity(new engine::Entity(
        "Grass Ground"s,
        nullptr,
        engine::Transform(
            glm::vec3(0.0f, -0.5f, 0.0f),
            glm::vec3(0.0f),
            planeSize
        ),
        new engine::ModelAttribute(grassGroundModel)
    ));

    // Add your model's entity here!
    scene->addEntity(new engine::Entity(
        "Red Apple 1"s,
        nullptr,
        engine::Transform(
            glm::vec3(-1.0f, 1.0f, -4.0f),
            glm::vec3(0.0f, 45.0f, 0.0f),
            0.05f
        ),
        new engine::ModelAttribute(redAppleModel)
    ));
    
    // Define lightings.
    // TODO : Maintain a light array in the scene.
    engine::DirectionalLight* sun = new engine::DirectionalLight(
        "Sun", 30.0f, 30.0f, glm::vec3(0.8f)
    );
    // Install our sun.
    gameManager.sun = sun;

    // Define depth texture.
    engine::DepthmapTexture* depthmap = new engine::DepthmapTexture(
        "Depth Map Texture",
        graphicsSettings.shadowWidth,
        graphicsSettings.shadowHeight
    );

    // Define the skybox cubemap.
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
    engine::Geometry* cubemapGeometry = new engine::Geometry(
        "Cubemap"s, "../resources/shape_primitive/skyboxP.json"s
    );
    scene->addGeometry(cubemapGeometry);
    scene->addEntity(new engine::Entity(
        "Skybox Cubemap"s,
        nullptr,
        engine::Transform(),
        new engine::CubemapAttribute(cubemapGeometry, skyboxTexture)
    ));

    // For debugging the shadow map.
    // engine::Geometry* quadGeometry = new engine::Geometry(
    //     "Simple Quad"s, "../resources/shape_primitive/quadPT.json"s
    // );
    // scene->addGeometry(quadGeometry);

    // Default shader parameters.
    lightingShader->use();
    lightingShader->setInt("material.diffuseSampler"s, 0);
    lightingShader->setInt("material.specularSampler"s, 1);
    lightingShader->setInt("material.normalSampler"s, 2);
    lightingShader->setInt("depthmapSampler"s, 3);
    lightingShader->setFloat("material.shininess"s, 64.0f); // Set shininess to constant value.

    skyboxShader->use();
    skyboxShader->setInt("skyboxSampler1"s, 0);

    // shadowDebugShader->use();
    // shadowDebugShader->setInt("depthSampler"s, 0);


    // Render loop.
    while (!glfwWindowShouldClose(window))
    {
        // Get current camera.
		engine::Camera* currentCamera = gameManager.defaultCamera;

        // Maintain loop time.
        timer.update((float)glfwGetTime());

        // Process input.
        processInput(window, sun);
        gameManager.processCommands(&cmd);

        // TODO : Render
        // (1) Render shadow map!
            // framebuffer: shadow frame buffer(depth.depthMapFBO)
            // shader : shadow.fs/vs
        glViewport(0, 0, graphicsSettings.shadowWidth, graphicsSettings.shadowHeight);

        glm::mat4 projection = sun->getProjectionMatrix();
        glm::mat4 view = sun->getViewMatrix(currentCamera->tf.position);
        glm::mat4 lightSpace = projection * view;

        shadowShader->use();
        shadowShader->setMat4("lightSpace"s, lightSpace);

        glBindFramebuffer(GL_FRAMEBUFFER, depthmap->FBO);
        // glClear(GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        for (const auto& entity : scene->entities)
        {
            engine::ModelAttribute* modelAttrib
                = entity.second->getAttribute<engine::ModelAttribute>();;
            if (modelAttrib && !(modelAttrib->model->ignoreShadow))
            {
                engine::Model* model = modelAttrib->model;
                glm::mat4 world = entity.second->tf.getModelMatrix();
                shadowShader->setMat4("world"s, world);
                model->bind();

                glDrawElements(
                    GL_TRIANGLES,
                    (GLsizei)(model->mesh->indices.size()),
                    GL_UNSIGNED_INT,
                    nullptr
                );
                glBindVertexArray(0);
            }
        }
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // (2) Render objects in the scene!
            // framebuffer : default frame buffer(0)
            // shader : shader_lighting.fs/vs
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, screen.width, screen.height);

        projection = currentCamera->getProjectionMatrix(
            screen.width, screen.height
        );
        view = currentCamera->getViewMatrix();

        lightingShader->use();
        lightingShader->setMat4("view"s, view);
        lightingShader->setMat4("projection"s, projection);
        lightingShader->setMat4("lightSpace"s, lightSpace);
        lightingShader->setVec3("viewPos"s, currentCamera->tf.position);
        lightingShader->setFloat(
            "useLighting"s, graphicsSettings.useLighting ? 1.0f : 0.0f
        );

        // TODO : Maybe allow some other light sources?
        // lightingShader->setVec3("light.position"s, -sun->lightDir);
        lightingShader->setVec3("light.direction"s, sun->lightDir);
        lightingShader->setVec3("light.color"s, sun->lightColor);

        lightingShader->setFloat(
            "useShadow"s, graphicsSettings.useShadow ? 1.0f : 0.0f
        );
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthmap->ID);
        for (const auto& entity : scene->entities)
        {
            engine::ModelAttribute* modelAttrib
                = entity.second->getAttribute<engine::ModelAttribute>();;
            if (modelAttrib)
            {
                engine::Model* model = modelAttrib->model;
                glm::mat4 world = entity.second->tf.getModelMatrix();
                lightingShader->setMat4("world"s, world);
                model->bind();

                bool useNormalMap = (model->normal != nullptr)
                    && graphicsSettings.useNormalMap;
                bool useSpecularMap = (model->specular != nullptr)
                    && graphicsSettings.useSpecularMap;
                lightingShader->setFloat("useNormalMap"s, useNormalMap ? 1.0f : 0.0f);
                lightingShader->setFloat("useSpecularMap"s, useSpecularMap ? 1.0f : 0.0f);
                glDrawElements(
                    GL_TRIANGLES,
                    (GLsizei)(model->mesh->indices.size()),
                    GL_UNSIGNED_INT,
                    nullptr
                );
                glBindVertexArray(0);
            }
        }

        // Render the skybox.
        skyboxShader->use();
        glDepthFunc(GL_LEQUAL);

        view = glm::mat4(glm::mat3(currentCamera->getViewMatrix()));
        skyboxShader->setMat4("view"s, view);
        skyboxShader->setMat4("projection"s, projection);

        glBindVertexArray(cubemapGeometry->VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture->ID);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // Log camera position.
        std::cout << std::setw(7) << std::setprecision(3)
            << currentCamera->tf.position.x << "  "
            << currentCamera->tf.position.y << "  "
            << currentCamera->tf.position.z << std::endl;

        // Debug shadow shader.
        // if (false)
        // {
        //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //     glViewport(0, 0, screen.width, screen.height);
        //
        //     shadowDebugShader->use();
        //     shadowDebugShader->setVec2("nearFar"s, glm::vec2(0.1f, 50.0f));
        //
        //     glBindVertexArray(quadGeometry->VAO);
        //     glActiveTexture(GL_TEXTURE0);
        //     glBindTexture(GL_TEXTURE_2D, depthmap->ID);
        //     glDrawElements(
        //         GL_TRIANGLES,
        //         quadGeometry->getSize(),
        //         GL_UNSIGNED_INT,
        //         nullptr
        //     );
        //     glBindVertexArray(0);
        // }

        // GLFW: Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional: De-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    delete scene;

    // GLFW: Terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Process all input: Query GLFW whether relevant keys are pressed/released this frame and react accordingly.
// ----------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, engine::DirectionalLight* sun)
{
    static float toggleNormalMapLastFrame = 0.0f;
    static float toggleSpecularMapLastFrame = 0.0f;
    static float toggleShadowLastFrame = 0.0f;
    static float toggleLightingLastFrame = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Make camera move by 3D translation (WASD, arrows, ascention using space).
    int moveForward = 0;
    int moveRight = 0;
    int moveUp = 0;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        ++moveForward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        --moveForward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        --moveRight;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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

    // TODO : 
    // Arrow key : increase, decrease sun's azimuth, elevation with amount of t.
    // key 1 : toggle using normal map
    // key 2 : toggle using shadow
    // key 3 : toggle using whole lighting
    int azimuthEast = 0;
    int elevationUp = 0;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        ++elevationUp;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        --elevationUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        --azimuthEast;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        ++azimuthEast;
    }

    float lastFrame = timer.getLastFrame();
    bool toggleNormalMap = false;
    bool toggleShadow = false;
    bool toggleLighting = false;
    bool toggleSpecularMap = false;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        if (toggleNormalMapLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            toggleNormalMap = true;
            toggleNormalMapLastFrame = lastFrame;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (toggleShadowLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            toggleShadow = true;
            toggleShadowLastFrame = lastFrame;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (toggleLightingLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            toggleLighting = true;
            toggleLightingLastFrame = lastFrame;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        if (toggleSpecularMapLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            toggleSpecularMap = true;
            toggleSpecularMapLastFrame = lastFrame;
        }
    }
        
    // Update the commands.
    cmd.moveForward = moveForward;
    cmd.moveRight = moveRight;
    cmd.moveUp = moveUp;
    cmd.sprint = sprint;

    cmd.azimuthEast = azimuthEast;
    cmd.elevationUp = elevationUp;

    graphicsSettings.useNormalMap = toggleNormalMap
        ? !graphicsSettings.useNormalMap : graphicsSettings.useNormalMap;
    graphicsSettings.useShadow = toggleShadow
        ? !graphicsSettings.useShadow : graphicsSettings.useShadow;
    graphicsSettings.useLighting = toggleLighting
        ? !graphicsSettings.useLighting : graphicsSettings.useLighting;
    graphicsSettings.useSpecularMap = toggleSpecularMap
        ? !graphicsSettings.useSpecularMap : graphicsSettings.useSpecularMap;
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
