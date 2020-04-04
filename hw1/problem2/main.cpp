#define GLM_ENABLE_EXPERIMENTAL

#define DEBUG
#define SCORE_TRIANGLE

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "shader.h"
#include "gameobjects.h"
#include "interfaces.h"


#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))  
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))  


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Command& cmd);


// setting
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "2019-20239 Jaerin Lee", NULL, NULL);
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
    Shader shader("shader.vs", "shader.fs");

    /////////////////////////////////////////////////////
    // TODO : Define VAO and VBO for triangle and quad(bar).
    /////////////////////////////////////////////////////
    
    // define triangle vertices
    float verticesCoin[] = {
        // positions; cog == (0, 0)
         0.0f,    0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
         0.433f, -0.25f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.433f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f // bottom left
    };

    // define bar vertices
    float verticesBar[] = {
        // positions; cog == (0, 0)
         0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.2f, // top right
        -0.5f,  0.5f, 0.0f, 1.0f, 0.2f, 0.2f, // top left
        -0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.2f, // bottom left
         0.5f, -0.5f, 0.0f, 1.0f, 0.2f, 0.2f, // bottom right
    };
    unsigned int indicesBar[] = {
        0, 1, 2, // first triangle
        0, 2, 3  // second triangle
    };

    unsigned int coinVBO;
    unsigned int coinVAO;

    unsigned int barVBO;
    unsigned int barVAO;
    unsigned int barEBO;

    glGenVertexArrays(1, &coinVAO);
    glGenBuffers(1, &coinVBO);

    glGenVertexArrays(1, &barVAO);
    glGenBuffers(1, &barVBO);
    glGenBuffers(1, &barEBO);

    glBindVertexArray(coinVAO);
    glBindBuffer(GL_ARRAY_BUFFER, coinVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoin), verticesCoin, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(barVAO);
    glBindBuffer(GL_ARRAY_BUFFER, barVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBar), verticesBar, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, barEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBar), indicesBar, GL_STATIC_DRAW); 

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // game entities
    Command cmd;
    std::vector<TriangleEntity> coins;
    Bar bar;


    // render loop
    // -----------
    float lastTime = 0.0f;
    float lastGeneratedTime = 0.0f;
    float generationInterval = 0.3f;
    unsigned int score = 0u;

    while (!glfwWindowShouldClose(window))
    {       
        // input
        // -----
        processInput(window, cmd);

        // clear background
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /////////////////////////////////////////////////////
        // TODO : Main Game Loop
        /////////////////////////////////////////////////////
        
        float curTime = glfwGetTime();
        float deltaLoopTime = curTime - lastTime; 
        float deltaGenTime = curTime - lastGeneratedTime;

        // process inputs
        float barPos = bar.position.x + cmd.transRight * bar.speed * deltaLoopTime;
        if ((barPos + 0.5f * bar.scale.x) > 1.0f)
            barPos = 1.0f - 0.5f * bar.scale.x;
        else if ((barPos - 0.5f * bar.scale.x) < -1.0f)
            barPos = -1.0f + 0.5f * bar.scale.x;
        bar.position.x = barPos;

        // (1) generate new triangle at the top of the screen for each time interval
        if (deltaGenTime > generationInterval)
        {
            lastGeneratedTime += generationInterval;
            TriangleEntity e = getRandomEntity();
            coins.push_back(e);
        }

        // (2) make triangles fall and rotate!!!!!
        // (3) Every triangle has different shape, falling speed, rotating speed.
        for (auto it = coins.begin(); it != coins.end(); ++it)
        {
            float rotation = it->rotation;
            it->rotation = fmod(rotation + it->rotationSpeed * deltaLoopTime, 360.0f);
            float y = it->position.y;
            y -= (it->dropSpeed * deltaLoopTime);
            it->position.y = y;

            // (5) Implement simple collision detection. Details are on the pdf file.
            if (y < -1.2f)
            {
                it->toDelete = true;
            }
            if (it->position.x > bar.position.x - 0.5f * bar.scale.x &&
                it->position.x < bar.position.x + 0.5f * bar.scale.x &&
                it->position.y > bar.position.y - 0.5f * bar.scale.y &&
                it->position.y < bar.position.y + 0.5f * bar.scale.y)
            {
                // you catched a coin!!!
                it->toDelete = true;
                ++score;
#ifdef DEBUG
                printf("%10.3f : You catched a coin! Total score = %u\n", curTime, score);
#endif
            }
            
            glm::mat4 transformMat =
                glm::translate(it->position)
                * glm::rotate(it->rotation, glm::vec3(0.0f, 0.0f, 1.0f))
                * glm::scale(glm::vec3(it->scale, it->scale, it->scale));

            // draw
            shader.use();
            shader.setMat4("transform", transformMat);

            glBindVertexArray(coinVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
        
        // (4) Render a red box
        glm::mat4 transformMat = glm::translate(bar.position) * glm::scale(bar.scale);

        shader.use();
        shader.setMat4("transform", transformMat);

        glBindVertexArray(barVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // delete the coins designated to be erased
        auto it = coins.begin();
        while (it != coins.end())
        {
            if (it->toDelete)
                it = coins.erase(it);
            else
                ++it;
        }

        // (6) Visualize the score.
#ifdef SCORE_TRIANGLE
        for (int i = 0; i < score && i < 20; ++i)
        {
            float posX = -0.9f + (i % 10) * 0.15f;
            float posY = -0.82f - (i / 10) * 0.15f;
            glm::mat4 transformMat =
                glm::translate(glm::vec3(posX, posY, 0.0f)) * glm::scale(glm::vec3(0.15f, 0.15f, 0.0f));

            // draw
            shader.use();
            shader.setMat4("transform", transformMat);

            glBindVertexArray(coinVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
#endif

        lastTime = curTime;
        cmd.reset();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    coins.clear();
    

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, Command& cmd)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //////////////////////////////////
    // TODO : make the bar movable
    //////////////////////////////////

    int transRight = 0;

    // 2d translation
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

    cmd.transRight = transRight;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
