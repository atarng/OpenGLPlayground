// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>   // Include GLEW
#include <glfw3.h>     // Include GLFW
#include <glm/glm.hpp> // Include GLM (MATH)
#include <glm/gtc/matrix_transform.hpp>

GLFWwindow* window;

/*** Custom Includes ***/
//#include <common/texture.hpp>
//#include <common/controls.hpp>
//#include <common/shader.hpp>
///////////////////////////////

//#include "ObjReader.h"
#include "SimpleGeometry.h"
#include "FPS_Update.h"

#include "TutorialObjLoading.h"
#include "TutorialShadowMapping.h"
#include "TutorialExtensions.h"
#include "TutorialBumpMapping.h"
#include "TutorialRenderToTexture.h"
/*** ***/

using namespace glm;
using namespace std;

int SetUpWindow() {
    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE,GL_FALSE); //

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible."
            "Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    ///
    // FRAME BUFFER WXH
    ///


    cout << "Press Escape to Exit Terminal" << endl;
    return 0;
}

void SetUpGLFWSettings() {

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //******* Tut 16 ********
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);
    //***************

    // Dark blue background
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

    /*** BEG: TUT 4C ***/
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    /*****/
    /*****/
}

int main( void )
{
    int to_render = 0;
    cout << "Enter a number (0-4): ";
    cin >> to_render;

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    if (SetUpWindow() < 0) {
        return -1;
    }

    SetUpGLFWSettings();

    /////
    ITutorialRenders* tr;
    switch (to_render) {
    case 4:
        tr = new TutorialRenderToTexture();
        break;
    case 3:
        tr = new TutorialBumpMapping();
        break;
    case 2:
        tr = new TutorialExtensions();
        break;
    case 1: 
        tr = new TutorialShadowMapping();
        break;
    case 0:
    default:
        tr = new TutorialObjLoading();
        break;
    }

    tr->Render_Loop_Setup(window);
    FPS_Update fu;
	do{
        fu.Loop();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /////
        tr->Render_Loop_Implementation();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    tr->Render_Loop_Cleanup();
    delete tr;
    tr = 0;

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}