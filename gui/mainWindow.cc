#include "mainWindow.h"
#include <thread>
#include <atomic>
#include "app.h"

extern std::atomic<bool> isRunning; //TODO:
extern std::atomic<bool> shouldExit;

GUI_MainWindow::GUI_MainWindow(QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);
    //Set HTML Text
    ui.textEdit->setHtml("<html><body>CHIP8-Emu</body></html>");
}

void runChip8Emu(const char *filename) {
    App app(filename);
    app.mainLoop();
    isRunning.store(false, std::memory_order_seq_cst);
    return;
}

void GUI_MainWindow::actionTestch8_clicked() {
    //Load Test ROM
    if (!isRunning.load(std::memory_order_seq_cst)) {
        isRunning.store(true, std::memory_order_seq_cst);
        shouldExit.store(false, std::memory_order_relaxed);
        std::thread t(runChip8Emu, "./rom/Pong (1 player).ch8");
        t.detach();
    }
}

//Close the current ROM
//TODO:
void GUI_MainWindow::actionReset_clicked() {
    shouldExit.store(true, std::memory_order_relaxed);
}