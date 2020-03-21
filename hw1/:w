#define DEBUG


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>

#ifdef DEBUG
#include <sstream>
#endif


#define PI 3.14159


// define states of the state machine
enum State
{
    STATE_OFF,
    STATE_ON,
};

// define input format
typedef struct _Commands
{
    int transRight; // 1: right -1: left 0: none
    int transUp; // 1: up -1: down 0: none
    int rotCcw; // 1: ccw -1: cw 0: none
} Commands;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Commands &cmd);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float SPEED_TRANS = 0.30f; // relative size
const float SPEED_ROT = 30.0f; // degrees

// project-related parameters
const float TRI_ON_TIME = 0.2f;
const float TRI_OFF_TIME = 0.8f;
glm::vec4 HERO_COLOR_ON(0.12f, 0.15f, 1.0f, 1.0f);
glm::vec4 HERO_COLOR_OFF(0.0f, 0.0f, 0.0f, 1.0f);

// current screen size
unsigned int screenHeight = SCR_HEIGHT;
unsigned int screenWidth = SCR_WIDTH;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shader.vs", "shader.fs"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions; cog == (0, 0)
         0.5f, -0.25f, 0.0f, // bottom right
        -0.5f, -0.25f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    //getPositionColorVAO(vertices, VAO, VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
    
    // our simple state machine
    State state = STATE_OFF;
    Commands cmd;
    float cx = 0.0f; // center x coord, relative width
    float cy = 0.0f; // center y coord, relative height
    float cth = 0.0f; // rotation around the center, radians

    // render loop
    // -----------
    float lastLoopTime = 0.0f;
    float lastStateUpdateTime = 0.0f;
    ourShader.setVec4("heroColor", HERO_COLOR_OFF);
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, cmd);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
        ourShader.use();
        float timeValue = glfwGetTime();

        /**********Fill in the blank*********/

        float deltaLoopTime = timeValue - lastLoopTime;
        float deltaStateUpdateTime = timeValue - lastStateUpdateTime;

        // color the triangle with respect to the periodic signals
        switch (state)
        {
        case STATE_OFF:
            if (deltaStateUpdateTime > TRI_OFF_TIME)
            {
                lastStateUpdateTime = timeValue;
                state = STATE_ON;
                ourShader.setVec4("heroColor", HERO_COLOR_ON);
            }
            break;
        case STATE_ON:
            if (deltaStateUpdateTime > TRI_ON_TIME)
            {
                lastStateUpdateTime = timeValue;
                state = STATE_OFF;
                ourShader.setVec4("heroColor", HERO_COLOR_OFF);
            }
            break;
        default:
            lastStateUpdateTime = timeValue;
            state = STATE_OFF;
            ourShader.setVec4("heroColor", HERO_COLOR_OFF);
            break;
        }

        // define spatial transform and apply it
        float dx = cmd.transRight * SPEED_TRANS * deltaLoopTime;
        float dy = cmd.transUp * SPEED_TRANS * deltaLoopTime;
        float dth = cmd.rotCcw * SPEED_ROT * deltaLoopTime;
            
        cx += dx;
        cy += dy;
        cth = fmod(cth + dth, 360.0f);
        ourShader.setVec3("transform", cx, cy, (cth / 180.0f + 0.5f) * PI);

        lastLoopTime = timeValue;

#ifdef DEBUG
        std::ostringstream cmdOss;
        cmdOss
            << (cmd.transUp == 1 ? "up" : cmd.transUp == -1 ? "down" : "")
            << (cmd.transRight == 1 ? "right" : cmd.transRight == -1 ? "left": "")
            << (cmd.rotCcw == 1 ? "ccw" : cmd.rotCcw == -1 ? "cw": "");

        printf("(%.3f %.3f %.3f)  ", cx, cy, cth);
        printf(
            "%.6f  %.6f  %.6f  %3s  %s\n",
            timeValue,
            deltaLoopTime,
            deltaStateUpdateTime,
            state == STATE_OFF ? "off" : "on",
            cmdOss.str().c_str()
        );
#endif

        /*************************************/

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Commands &cmd)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    /**********Fill in the blank*********/

    int transUp = 0;
    int transRight = 0;
    int rotCcw = 0;

    // 2d translation
    if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS))
    {
        ++transUp;
    }
    if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS))
    {
        --transUp;
    }
    if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS))
    {
        --transRight;
    }
    if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS))
    {
        ++transRight;
    }

    // 2d rotation
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        ++rotCcw;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        --rotCcw;
    }

    cmd.transUp = transUp;
    cmd.transRight = transRight;
    cmd.rotCcw = rotCcw;

    /*************************************/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    screenHeight = height;
    screenWidth = width;
}
