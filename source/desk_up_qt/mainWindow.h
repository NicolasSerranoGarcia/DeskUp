#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddWorkspace();
    void onRestoreWorkspace();
    void onExit();
    void onAbout();

private:
    void setupMenus();

    static void showSaveSuccessful();
    static void showRestoreSuccessful();
};

#endif