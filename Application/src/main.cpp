#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include <stdio.h>
#include "Application.h"
#include "freeimage.h"
#include <GL/freeglut_std.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void PrintOpenGLVersion();


int main (int argc, char** argv) 
{
    //glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    //glutCreateWindow("Test");



    FreeImage_Initialise();
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
        fprintf (stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }
    
#ifdef __APPLE__
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
    const int WindowWidth = 1400;
    const int WindowHeight = 800;
    
    GLFWwindow* window = glfwCreateWindow (WindowWidth, WindowHeight, "Computergrafik - Hochschule Osnabrück", NULL, NULL);
    if (!window) {
        fprintf (stderr, "ERROR: can not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent (window);

    // ImGui
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //
    //ImFont* pFont = io.Fonts->AddFontFromFileTTF("C:\Windows\Fonts\Arial.ttf", 50);
    //ImGui::PushFont(pFont);


#if WIN32
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    PrintOpenGLVersion();

    // ImGui inistialisieren
    //const char* glsl_version = "#version 130";
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();
    //(void)io;

    //ImGui::StyleColorsDark();
    //ImGui_ImplGlfw_InitForOpenGL(window, true);
    //ImGui_ImplOpenGL3_Init(glsl_version);
    float LastFrametime = 0;

    
    {
        Application App(window);
        App.start();
        while (!glfwWindowShouldClose (window)) {
            float DeltaTime = glfwGetTime() - LastFrametime;
            LastFrametime = glfwGetTime();
            // once per frame
            glfwPollEvents();
            App.update((float)DeltaTime);
            App.draw();
            glfwSwapBuffers (window);
        }
        App.end();
    }
    
    glfwTerminate();
    return 0;
}


void PrintOpenGLVersion()
{
    // get version info
    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);
}
