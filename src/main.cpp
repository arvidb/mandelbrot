#include "common.h"
#include "mandelbrot.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <random>

bool initGL(GLFWwindow** window, int& /*out*/screenWidth, int& /*out*/screenHeight) {
    
    glfwSetErrorCallback([](int /*error*/, const char* description) {
        std::cerr << "Error: " << description << '\n';
    });
    
    if (glfwInit() != GLFW_TRUE) {
        return false;
    }
    
    auto monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    
    screenWidth = mode->width;
    screenHeight = mode->height;
    
    *window = glfwCreateWindow(screenWidth, screenHeight, "Simple example", monitor, nullptr);
    if (*window == nullptr)
    {
        glfwTerminate();
        return false;
    }
    
    glfwSetKeyCallback(*window, [](GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    
    glfwMakeContextCurrent(*window);
    glfwSwapInterval(1);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, screenWidth, screenHeight, 0, 1, -1);
    
    return true;
}

int main()
{
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_real_distribution<float> distr(0, 1);
    
    int screenWidth, screenHeight;
    GLFWwindow *window;
    
    if (!initGL(&window, screenWidth, screenHeight)) {
        exit(EXIT_FAILURE);
    }
    
    Mandelbrot<double> mandelbrot(screenHeight, screenHeight, 50);
    
    // Genrate a new random color
    mandelbrot.setColor(vector3(distr(generator), distr(generator), distr(generator)));

    // Create a texture
    GLuint tex;
    glGenTextures(1, &tex);
    
    double lastTime = 0;
    const double maxFramerate = 30.0f;
    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        auto currentTime = glfwGetTime();
        if(currentTime - lastTime >= 1.0 / maxFramerate)
        {
            lastTime = currentTime;
            
            if (!mandelbrot.isReady()) {
                continue;
            }
            else {
                mandelbrot.generate();
            }
            
            // Draw
            
            glClearColor(0, 0, 0, 1.0);
            
            // Bind texture to mandelbrot buffer
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mandelbrot.getWidth(), mandelbrot.getHeight(), 0, GL_RGB, GL_FLOAT, mandelbrot.getBuffer());

            // Draw texture
            glClear(GL_COLOR_BUFFER_BIT);
            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2i(0, 0);
            glTexCoord2i(0, 1); glVertex2i(0, screenHeight);
            glTexCoord2i(1, 1); glVertex2i(screenWidth, screenHeight);
            glTexCoord2i(1, 0); glVertex2i(screenWidth, 0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            glfwSwapBuffers(window);
        }
        
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    exit(EXIT_SUCCESS);
}
