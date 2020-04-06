/**
 * CHIP-8 Emulator
 * 
 */

#include <iostream>
#include <cstdlib>
#include <atomic>

#include <QApplication>
#include "gui/mainWindow.h"

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// #define _DEBUG
// #define USE_NVIDIA_OPTIMUS

#include "app.h"
#include "gui/qdebugstream.h"

#ifdef USE_NVIDIA_OPTIMUS
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

std::atomic<bool> isRunning; //TODO:
std::atomic<bool> shouldExit;

int main(int argc, char *argv[]) {

    SDL_SetMainReady();
 
    std::ios_base::sync_with_stdio(false);

    isRunning.store(false, std::memory_order_seq_cst);
    shouldExit.store(false, std::memory_order_relaxed);

    QApplication qt_app(argc, argv);
    GUI_MainWindow gui_mainWindow;

    gui_mainWindow.show();
    return qt_app.exec();
}