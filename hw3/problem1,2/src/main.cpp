#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "system_global.h"
#include "engine_global.h"

#include "camera.h"
#include "geometry.h"
#include "math_utils.h"
#include "shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


int main()
{
    // GLFW: Initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Uncomment this statement to fix compilation on OS X
#endif

    // GLFW window creation
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

    // GLAD: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // Build and compile our shader program
    // ------------------------------------
    // Define 3 shaders
    // (1) Geometry shader for spline render.
    // (2) Simple shader for spline's outer line render.
    // (3) Tessellation shader for bezier surface.
    engine::Shader splineShader(
        "../shaders/splines/spline_shader.vert",
        "../shaders/splines/spline_shader.frag",
        "../shaders/splines/spline_shader.geom"
    );
    engine::Shader splineOutlineShader(
        "../shaders/splines_outline/outer_line_shader.vert",
        "../shaders/splines_outline/outer_line_shader.frag"
    );
    /*
    engine::Shader bezierSurfaceShader(
        "../shaders/bezier_surface/tess.vert",
        "../shaders/bezier_surface/tess.frag",
        "../shaders/bezier_surface/tess.geom",
        "../shaders/bezier_surface/tess.tesc",
        "../shaders/bezier_surface/tess.tese"
    );
    */

    // Load requied model and save data to VAO. 
    // Note : loadSplineControlPoints/loadBezierSurfaceControlPoints in
    //        <resource_utils.h> are used in the <geometry.h>
    engine::Geometry *lineSimpleGeometry = new engine::Geometry(
        "Spline Simple",
        "../resources/spline_control_point_data/spline_simple.txt",
        engine::GeometryDataMode::LINE
    );
    engine::Geometry *lineLetterUGeometry = new engine::Geometry(
        "Spline Letter U",
        "../resources/spline_control_point_data/spline_u.txt",
        engine::GeometryDataMode::LINE
    );
    engine::Geometry *lineComplexGeometry = new engine::Geometry(
        "Spline Complex",
        "../resources/spline_control_point_data/spline_complex.txt",
        engine::GeometryDataMode::LINE
    );

    /*
    engine::Geometry *surfaceHeart = new engine::Geometry(
        "Bezier Heart",
        "../resources/bezier_surface_data/heart.bpt",
        engine::GeometryDataMode::SURFACE
    );
    engine::Geometry *surfaceGumbo = new engine::Geometry(
        "Bezier Gumbo",
        "../resources/bezier_surface_data/gumbo.bpt",
        engine::GeometryDataMode::SURFACE
    );
    engine::Geometry *surfaceTeapot = new engine::Geometry(
        "Bezier Teapot",
        "../resources/bezier_surface_data/teapot.bpt",
        engine::GeometryDataMode::SURFACE
    );
    engine::Geometry *surfaceSphere = new engine::Geometry(
        "Bezier Sphere",
        "../resources/bezier_surface_data/sphere.bpt",
        engine::GeometryDataMode::SURFACE
    );
    */

    glm::vec3 lineSimpleBezierPosition = glm::vec3(-4.0, 0.0, -1.0);
    glm::vec3 lineSimpleBSplinePosition = glm::vec3(-2.4, 0.0, -1.0);
    glm::vec3 lineSimpleCatmullRomPosition = glm::vec3(-0.8, 0.0, -1.0);
    glm::vec3 lineLetterUBezierPosition = glm::vec3(1.0, -1.8, -1.0);
    glm::vec3 lineComplexBSplinePosition = glm::vec3(3.0, 0.0, -1.0);
    glm::vec3 lineComplexCatmullRomPosition = glm::vec3(5.0, 0.0, -1.0);

    struct SplineObject
    {
        engine::Geometry *geometry;
        glm::vec3 position;
        glm::vec3 scale;
        GLenum splinePrimitive;
        GLenum outlinePrimitive;
        unsigned subroutineIndex;
    };
    std::vector<SplineObject> splineObjects = {
        { lineSimpleGeometry, lineSimpleBezierPosition, glm::vec3(1.0f), GL_LINE_STRIP_ADJACENCY, GL_LINE_STRIP, 0 },
        { lineSimpleGeometry, lineSimpleBSplinePosition, glm::vec3(1.0f), GL_LINE_STRIP_ADJACENCY, GL_LINE_STRIP, 1 },
        { lineSimpleGeometry, lineSimpleCatmullRomPosition, glm::vec3(1.0f), GL_LINE_STRIP_ADJACENCY, GL_LINE_STRIP, 2 },
        { lineLetterUGeometry, lineLetterUBezierPosition, glm::vec3(0.2f), GL_LINES_ADJACENCY, GL_LINE_STRIP, 0 },
        { lineComplexGeometry, lineComplexBSplinePosition, glm::vec3(0.8f), GL_LINE_STRIP_ADJACENCY, GL_LINE_STRIP, 1 },
        { lineComplexGeometry, lineComplexCatmullRomPosition, glm::vec3(0.8f), GL_LINE_STRIP_ADJACENCY, GL_LINE_STRIP, 2 }
    };
    std::vector<std::string> subroutineNames = {
        "interpBezier", "interpBSpline", "interpCatmullRomUniform",
        "interpCatmullRomCentripetal", "interpCatmullRomChordal"
    };

    // Render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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

        // Render splines
        // (1) Render simple spline with 4 control points for Bezier, Catmull-Rom and B-spline.
        // (2) Render 'u' using Bezier spline
        // (3) Render loop using Catmull-Rom spline and B-spline.
        // You have to also render outer line of control points!
        
        glm::mat4 projection = currentCamera->getProjectionMatrix(screen.width, screen.height);
        glm::mat4 view = currentCamera->getViewMatrix();

        splineShader.use();
        GLint splineSubroutineCount = 0;
        glGetProgramStageiv(
            splineShader.ID,
            GL_GEOMETRY_SHADER,
            GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            &splineSubroutineCount
        );

        for (const auto &obj : splineObjects)
        {
            splineShader.use();
            splineShader.setMat4("projection", projection);
            splineShader.setMat4("view", view);
            glm::mat4 model = glm::scale(obj.scale);
            model = glm::translate(model, obj.position);
            splineShader.setMat4("model", model);
            
            GLuint subroutineIndex = glGetSubroutineIndex(
                splineShader.ID,
                GL_GEOMETRY_SHADER,
                subroutineNames[obj.subroutineIndex].c_str()
            );
            glUniformSubroutinesuiv(
                GL_GEOMETRY_SHADER,
                splineSubroutineCount,
                &subroutineIndex
            );
            glBindVertexArray(obj.geometry->VAO);
            glDrawArrays(obj.splinePrimitive, 0, obj.geometry->numVertices);

            if (cmd.splineOuterLineOn)
            {
                // Render outer line for splines.
                splineOutlineShader.use();
                splineOutlineShader.setMat4("projection", projection);
                splineOutlineShader.setMat4("view", view);
                splineOutlineShader.setMat4("model", model);
                glDrawArrays(obj.outlinePrimitive, 0, obj.geometry->numVertices);
            }
        }

        // TODO : Render Bezier surfaces using tessellation shader.
        

        std::cout << std::setw(7) << std::setprecision(3)
            << currentCamera->position.x << "  " << currentCamera->position.y << "  " << currentCamera->position.z << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Optional: De-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    free(lineSimpleGeometry);
    free(lineLetterUGeometry);
    free(lineComplexGeometry);

    /*
    free(surfaceHeart);
    free(surfaceGumbo);
    free(surfaceTeapot);
    free(surfaceSphere);
    */

    // GLFW: Terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    static float splineOuterLineToggleLastFrame = 0.0f;
    static float surfaceFillToggleLastFrame = 0.0f;

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

    // (1) (For spline) If we press key 9, toggle whether to render outer line.
    // (2) (For Bezier surface) If we press key 0, toggle GL_FILL and GL_LINE.
    float lastFrame = timer.getLastFrame();
    bool splineOuterLineToggle = false;
    bool surfaceFillToggle = false;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        if (splineOuterLineToggleLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            splineOuterLineToggle = true;
            splineOuterLineToggleLastFrame = lastFrame;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
        if (surfaceFillToggleLastFrame + engine::KEYBOARD_TOGGLE_DELAY < lastFrame)
        {
            surfaceFillToggle = true;
            surfaceFillToggleLastFrame = lastFrame;
        }
    }
    
    // Update the commands.
    cmd.moveForward = moveForward;
    cmd.moveRight = moveRight;
    cmd.moveUp = moveUp;
    cmd.sprint = sprint;
    cmd.splineOuterLineOn = splineOuterLineToggle ? !cmd.splineOuterLineOn : cmd.splineOuterLineOn;
    cmd.surfaceFillOn = surfaceFillToggle ? !cmd.surfaceFillOn : cmd.surfaceFillOn;
}

// GLFW: Whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    screen.width = width;
    screen.height = height;

    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// GLFW: Whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    gameManager.processMouseScroll(xoffset, yoffset);
}
