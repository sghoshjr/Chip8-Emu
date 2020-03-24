#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "ui_mainWindow.h"
#include <QString>

class GUI_MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit GUI_MainWindow(QMainWindow *parent = nullptr);

  private slots:
    void actionTestch8_clicked();
    void actionReset_clicked();

  private:
    Ui::MainWindow ui;
};

#endif // GUI_MAINWINDOW_H