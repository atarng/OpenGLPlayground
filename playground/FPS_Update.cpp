#include "FPS_Update.h"
FPS_Update::FPS_Update() {
    lastTime = glfwGetTime();
    nbFrames = 0;
}
void FPS_Update::Loop() {
    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >= 1.0)
    { // If last prinf() was more than 1sec ago; printf and reset
        printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }
}