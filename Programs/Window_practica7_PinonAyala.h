#pragma once
#include<stdio.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

class Window
{
public:
    Window();
    Window(GLint windowWidth, GLint windowHeight);
    int Initialise();
    GLfloat getBufferWidth() { return bufferWidth; }
    GLfloat getBufferHeight() { return bufferHeight; }
    GLfloat getXChange();
    GLfloat getYChange();
    GLfloat getmuevex() { return muevex; }
    GLfloat getGira() { return Gira; }
    GLfloat getDespega() { return despega; }
    GLfloat getArranca() { return arranca; }
    GLboolean getLucesDelante() { return LucesDelante; }
    GLboolean getTerceraPersona() { return terceraPersona; }
    GLboolean getAnimated() { return animated; }

    bool getShouldClose() {
        return  glfwWindowShouldClose(mainWindow);}
    bool* getsKeys() { return keys; }
    void swapBuffers() { return glfwSwapBuffers(mainWindow); }
    
    ~Window();
private:
    GLFWwindow *mainWindow;
    GLint width, height;
    bool keys[1024];
    GLint bufferWidth, bufferHeight;
    void createCallbacks();
    GLfloat lastX;
    GLfloat lastY;
    GLfloat xChange;
    GLfloat yChange;
    GLfloat muevex;
    GLfloat Gira;
    GLfloat despega;
    GLboolean LucesDelante;
    GLboolean arranca;
    GLboolean terceraPersona;
    GLboolean animated;
    
    bool mouseFirstMoved;
    static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
    static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

