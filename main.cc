/**
 * CHIP-8 Emulator
 * 
 */

#include <iostream>
#include <cstdlib>

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// #define _DEBUG
// #define USE_NVIDIA_OPTIMUS

#include "app.h"

#ifdef USE_NVIDIA_OPTIMUS
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

bool enableANSIColorCode = true;

#if defined(_WIN32)
DWORD enableVirtualTerminalProcessing() {

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) {
        return GetLastError();
    }
    return 0;
}
#endif

void runChip8Emu(const char *filename) {
    App app(filename);
    app.mainLoop();
    return;
}

int main(int argc, char const *argv[]) {

    SDL_SetMainReady();

    if (argc != 2) {
        std::cout << "run as ./" << argv[0] << " <rom-file>" << std::endl;
    }
 
    std::ios_base::sync_with_stdio(false);
    
    #if defined(_WIN32)
    DWORD err = enableVirtualTerminalProcessing();
    if (err) {
        std::cerr << "Could not enable Virtual Terminal Processing" << std::endl;
        enableANSIColorCode = false;
    }
    #endif

    if (argc == 2) {
        runChip8Emu(argv[1]);
    } else {
        //Test
        runChip8Emu("./rom/Pong (1 player).ch8");
        // runChip8Emu("./rom/Tetris [Fran Dachille, 1991].ch8");
    }
    
    std::cout << "\nPress any key to Exit..." << std::endl; //TODO: Remove later
    std::cin.get();
    return 0;
}