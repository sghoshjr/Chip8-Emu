#include <thread>
#include <atomic>
#include <string>
#include <filesystem>

#include "mainWindow.h"
#include "app.h"
#include "qdebugstream.h"
#include "utils/configReader.h"

extern std::atomic<bool> isRunning;  //TODO:
extern std::atomic<bool> shouldExit;

GUI_MainWindow::GUI_MainWindow(const char *argv0, QMainWindow *parent) : QMainWindow(parent) {
    ui.setupUi(this);

    //Set HTML Text
    //TODO: Format Text
    ui.textEdit->setText("CHIP8-Emu");

    //Redirect stdout and stderr to textEdit
    log_stdout = std::make_shared<ThreadLogStream>(std::cout, this->ui.textEdit);
    log_stderr = std::make_shared<ThreadLogStream>(std::cerr, this->ui.textEdit);

    //Read Config File
    auto path = std::filesystem::path(argv0).parent_path();
    path += path.preferred_separator;
    config = std::make_unique<configReader>(path.string());

    //Add Recent ROMS to Open ROMs
    for (int i = 0; i < config->getRecentROMSize(); ++i) {
        std::string actionObjName = "actionRecentROM_";
        actionObjName += std::to_string(i);

        v_recentROMS.push_back(std::make_shared<QAction>(this));
        v_recentROMS.back()->setObjectName(actionObjName.c_str());
        v_recentROMS.back()->setText(QApplication::translate("MainWindow", config->getRecentROM(i).c_str(), Q_NULLPTR));
        
        if (config->getRecentROM(i).empty()) {
            v_recentROMS.back()->setVisible(false);
        }

        ui.menuOpen_ROM->addAction(v_recentROMS.back().get());
        //TODO: temporary slot func for now
        QObject::connect(v_recentROMS.back().get(), SIGNAL(triggered()), this, SLOT(actionTestch8_clicked()));
        recentRomId[actionObjName] = i;
    }
}

void runChip8Emu(std::string filename) {
    App app(filename.c_str());
    app.mainLoop();
    isRunning.store(false, std::memory_order_seq_cst);
    return;
}

void GUI_MainWindow::actionTestch8_clicked() {
    //Load Test ROM
    QObject* obj = QObject::sender();
    auto id = recentRomId[obj->objectName().toStdString()];

    if (!isRunning.load(std::memory_order_seq_cst)) {
        isRunning.store(true, std::memory_order_seq_cst);
        shouldExit.store(false, std::memory_order_relaxed);
        //TODO: check valid rom
        std::thread t(runChip8Emu, config->getRecentROM(id));  //"./rom/Pong (1 player).ch8"
        t.detach();
    }
}

//Close the current ROM
//TODO:
void GUI_MainWindow::actionReset_clicked() {
    shouldExit.store(true, std::memory_order_relaxed);
}