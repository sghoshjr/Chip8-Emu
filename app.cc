#include "app.h"

#include <atomic>

extern std::atomic<bool> isRunning; //TODO:
extern std::atomic<bool> shouldExit;

App::App(const char *filename) {
    clock_msec = (1000/clock_hz);

    initializeGLFW();
    initializeSDL();
    setGLFWCallback();
    setupObject();

    chip8Console.loadROM(filename); //TODO: Exit if failed to load
}

App::~App() {
    glUseProgram(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(shaderID);
    glDeleteTextures(1, &textureID);
    glfwTerminate();

    SDL_ClearQueuedAudio(audioDeviceID);
    SDL_CloseAudio();
    SDL_Quit();
}

// Initialize GLFW and set context
void App::initializeGLFW() {
    glfwInit();
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    #ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    #endif
    
    ratio = (1.0*VIDEO_WIDTH)/VIDEO_HEIGHT;
    screenWidth = VIDEO_WIDTH*scale;
    screenHeight = VIDEO_HEIGHT*scale;

    monitor = glfwGetPrimaryMonitor();
    window = glfwCreateWindow(screenWidth, screenHeight, "CHIP8-Emu", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    
    glewExperimental = true;
    glewInit();

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwPollEvents();
    glfwSetCursorPos(window, 0, 0);
    glfwSwapInterval(0);
}

// Set Up GLFW Callbacks
void App::setGLFWCallback() {
    glfwSetKeyCallback(window, ::keyCallback);
    glfwSetFramebufferSizeCallback(window, ::frameBufferResizeCallback);
    
    #ifdef _DEBUG
    if (glDebugMessageCallback != NULL) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback((GLDEBUGPROC)debugCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "GL_KHR_debug Supported" << std::endl;
    }
    #endif
}

// Set up VAO, Texture, VBO, Shader
void App::setupObject() {
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);

    //VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    //Setup Texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VIDEO_WIDTH, VIDEO_HEIGHT,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, chip8Console.video_frame);

    //VBO
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frame_vertex), frame_vertex, GL_STATIC_DRAW);

    //Load Program
    shaderID = LoadShaders("shaders/vertex.glsl", "shaders/frag.glsl");
}

//Draw video_frame Output
void App::draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw Frame
    glUseProgram(shaderID);
    glBindVertexArray(vertexArrayID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT, 
                    GL_RGBA, GL_UNSIGNED_BYTE, chip8Console.video_frame);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glGenerateMipmap(GL_TEXTURE_2D);

    glUniform1i(glGetUniformLocation(shaderID, "textureSampler"), 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
}

// GLFW Key Callback
void App::keyCallback(int key, int scancode, int action, int mods) {
    auto idx = std::distance(keyMapping.begin(), std::find(keyMapping.begin(), keyMapping.end(), key));
    
    if (action == GLFW_PRESS) {
        chip8Console.keypad[idx] = 1;
    } else if (action == GLFW_RELEASE) {
        chip8Console.keypad[idx] = 0;
    }

    if (key == GLFW_KEY_ENTER) {
        chip8Console.reset();
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, 1);
    }
}

// GLFW Frame Buffer resize Callback
void App::frameBufferResizeCallback(int width, int height) {
    screenWidth  = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
    ratio = (float)width/height;
}

// Pseudo GLFW Callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App *app_ptr = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    app_ptr->keyCallback(key, scancode, action, mods);
}

void frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
    App *app_ptr = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    app_ptr->frameBufferResizeCallback(width, height);
}

// Initialize SDL and generate sound wave
void App::initializeSDL() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cout << "Could not Initialize SDL" << std::endl;
        exit(1);
    }

    audioSpec.callback = NULL;
    audioSpec.channels = 1;
    audioSpec.format = AUDIO_S8;
    audioSpec.freq = SAMPLING_FREQ;
    audioSpec.samples = SAMPLE;

    generateSineWave(700, AMP);
    
    audioDeviceID = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
    SDL_PauseAudioDevice(audioDeviceID, 0);
}

// Generate Sound of specified freq and amp
void App::generateSineWave(double freq, double amp) {
    for (int i = 0; i < audioLength; ++i) {
        audioBuffer[i] = (int8_t)(amp * std::sin((2 * PI * freq * i)/SAMPLING_FREQ));
    }
}

// Play sound
void App::beep() {
    SDL_QueueAudio(audioDeviceID, audioBuffer.data(), audioLength);
}

//Main Loop
void App::mainLoop() {
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    std::stringstream ss;
    bool playSound = false;

    float fps = 0.0;
    auto lastTime = std::chrono::high_resolution_clock::now();

    do {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time_diff = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        
        chip8Console.cycle();

        ++fps;
        auto time_interval = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTime).count();

        if (time_interval >= 1000.0f) {
            ss << "CHIP8-Emu : " << std::fixed << std::setprecision(2) << fps << " FPS";
            glfwSetWindowTitle(window, ss.str().c_str());
            fps = 0;
            lastTime = currentTime;
            ss.str("");
            ss.clear();
        }

        if (time_diff >= clock_msec) {
            playSound = chip8Console.clock_tick();
            if (playSound) {
                beep();
            }

            lastCycleTime = currentTime;
        }

        // if (chip8Console.draw) {
        draw();
        SDL_Delay(1);
            // chip8Console.draw = false;
        // }

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && 
             glfwWindowShouldClose(window) == 0 &&
             !shouldExit.load(std::memory_order_relaxed));
}






