#ifndef APP_H
#define APP_H

#include <array>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "shader_utils.h"
#include "chip8.h"

// Sound
constexpr double SAMPLING_FREQ = 44100;
constexpr double AMP = std::numeric_limits<int8_t>::max();
constexpr double SND_TIME = 0.100;
constexpr int SAMPLE = 4096;
constexpr int AUDIO_LENGTH = 1 * SND_TIME * SAMPLING_FREQ; //8-bit Audio
constexpr double PI = 3.1415926535897;

class App {
  private:
    GLFWwindow *window = nullptr;
    GLFWmonitor *monitor = nullptr;
    bool fullscreen = false;

    int screenWidth, screenHeight;
    float ratio;
    int scale = 10;

    GLfloat frame_vertex[3*6] = {
        -1.0f, -1.0f, 0.0f,     // Rectangle
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    GLuint textureID = 0;
    GLuint vertexArrayID = 0;
    GLuint vertexBuffer = 0;
    GLuint shaderID = 0;

    SDL_AudioDeviceID audioDeviceID = 0;
    SDL_AudioSpec audioSpec{};
    uint32_t audioLength = AUDIO_LENGTH;
    std::array<int8_t, AUDIO_LENGTH> audioBuffer{};

  public:
    Chip8 chip8Console;

    double clock_hz = 60.;
    double clock_msec = (1000/60.);

    std::array<uint8_t, 16U> keyMapping = 
    {
        'X', '1', '2', '3', 
        'Q', 'W', 'E', 'A', 
        'S', 'D', 'Z', 'C',
        '4', 'R', 'F', 'V',
    };

    App(const char *filename);
    ~App();

    //OpenGL and GLFW
    void initializeGLFW();
    void setGLFWCallback();
    void setupObject();
    void draw();

    //GLFW Callbacks
    void keyCallback(int key, int scancode, int action, int mods);
    void frameBufferResizeCallback(int width, int height);

    //SDL Sound Playback
    void initializeSDL();
    void generateSineWave(double freq, double amp);
    void beep();

    void mainLoop();
};

//Pseudo-GLFW callbacks
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

#endif //APP_H