#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <vector>
#include <memory>
#include <map>
#include "ui_mainWindow.h"
#include <QString>
#include "qdebugstream.h"
#include "utils/configReader.h"

class GUI_MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit GUI_MainWindow(const char *argv0, QMainWindow *parent = nullptr);

  private slots:
    void actionTestch8_clicked();
    void actionReset_clicked();

  private:
    Ui::MainWindow ui;
    std::shared_ptr<ThreadLogStream> log_stdout;
    std::shared_ptr<ThreadLogStream> log_stderr;

    std::unique_ptr<configReader> config;
    std::vector<std::shared_ptr<QAction>> v_recentROMS;
    std::map<std::string, int> recentRomId;
};

#endif  // GUI_MAINWINDOW_H