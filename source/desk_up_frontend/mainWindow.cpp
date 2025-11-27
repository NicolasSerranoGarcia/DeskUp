#include "MainWindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include <QAction>
#include <QStatusBar>

#include "desk_up_error.h"
#include "window_core.h"
#include "desk_up_backend_interface.h"
#include "desk_up_error_gui_converter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("DeskUp");
    resize(800, 600);

    setupMenus();

    statusBar()->showMessage("Welcome to DeskUp!");

    #ifdef _WIN32
        setWindowIcon(QIcon("../assets/DeskUp.ico"));
    #endif

    if (!DU_Init()) {
        QMessageBox::critical(this, "DeskUp error",
            "There was an error initializing DeskUp. Try closing and reopening the app.");
    }
}

void MainWindow::setupMenus()
{
    QMenu *workspaceMenu = menuBar()->addMenu("&Workspace");
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *addAct = new QAction("Add workspace\tCtrl+N", this);
    QAction *restoreAct = new QAction("Restore workspace\tCtrl+R", this);
    QAction *exitAct = new QAction("Exit", this);
    QAction *aboutAct = new QAction("About", this);

    workspaceMenu->addAction(addAct);
    workspaceMenu->addAction(restoreAct);
    workspaceMenu->addSeparator();
    workspaceMenu->addAction(exitAct);
    helpMenu->addAction(aboutAct);

    connect(addAct, &QAction::triggered, this, &MainWindow::onAddWorkspace);
    connect(restoreAct, &QAction::triggered, this, &MainWindow::onRestoreWorkspace);
    connect(exitAct, &QAction::triggered, this, &MainWindow::onExit);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::onExit(){
    close();
}

void MainWindow::onAbout(){
    QString info = R"(
        <h3>DeskUp</h3>
        <p>Version: )" + QString(DESKUP_VERSION) + R"(</p>
        <p>A workspace manager for Windows that lets you save and restore window layouts.</p>
        <p>© 2025 Nicolas Serrano Garcia</p>
        <p>Licensed under GPLv3. See LICENSE for details.</p>
        <p><a href="https://github.com/NicolasSerranoGarcia/DeskUp">GitHub Repository</a></p>
    )";

    QMessageBox::about(this, "About DeskUp", info);
}

void MainWindow::onAddWorkspace(){
    QString workspaceName;

    while (true) {
        bool ok;
        workspaceName = QInputDialog::getText(this, "Add workspace",
                                              "Enter the workspace name:", QLineEdit::Normal, "", &ok);

        if (!ok){
			return;
		}

        if (workspaceName.isEmpty()) {
            QMessageBox::critical(this, "Workspace name", "The workspace name cannot be empty!");
            continue;
        }

        break;
    }

    const std::string ws = workspaceName.toStdString();

    if (!DeskUpBackendInterface::existsWorkspace(ws)) {
        if (auto saveRes = DeskUpBackendInterface::saveAllWindowsLocal(ws); !saveRes.has_value()) {

			//this should take care of showing the message depending on the severity, and then outside the function close the program if needed
			DeskUp::UI::ErrorAdapter::showError(std::move(saveRes.error()));

			auto saveErr = saveRes.error();
			if(saveErr.isFatal()){
				//for now just close the program

				// QApplication::exit();
				exit(1);
			}

        } else {
            showSaveSuccessful();
        }
    } else {
        QMessageBox::StandardButton response = QMessageBox::question(
            this, "Workspace exists",
            "The workspace already exists. Do you wish to rewrite it?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
            QMessageBox::No);

        if (response == QMessageBox::Yes) {
            DeskUpBackendInterface::removeWorkspace(ws);

            if (auto res = DeskUpBackendInterface::saveAllWindowsLocal(ws); !res.has_value()) {
                DeskUp::UI::ErrorAdapter::showError(std::move(res.error()));
            }
        }
    }
}

void MainWindow::onRestoreWorkspace()
{
    QString workspaceName;

    while (true) {
        bool ok;
        workspaceName = QInputDialog::getText(this, "Restore workspace",
                                              "Enter the workspace name:", QLineEdit::Normal, "", &ok);

        if (!ok)
            return;

        if (workspaceName.isEmpty()) {
            QMessageBox::critical(this, "Workspace name", "The workspace name cannot be empty!");
            continue;
        }

        if (!DeskUpBackendInterface::isWorkspaceValid(workspaceName.toStdString())) {
            QMessageBox::critical(this, "Workspace name not valid",
                                  "The workspace name is not valid! Blacklisted characters: \\/:?*\"<>|");
            continue;
        }
        break;
    }

    const std::string ws = workspaceName.toStdString();

    if (DeskUpBackendInterface::existsWorkspace(ws)) {
        if (auto res = DeskUpBackendInterface::restoreWindows(ws); !res.has_value()) {
            DeskUp::UI::ErrorAdapter::showError(std::move(res.error()));

			auto restoreErr = res.error();
			if(restoreErr.isFatal()){
				//for now just close the program

				// QApplication::exit();
				exit(1);
			}
        } else {
            showRestoreSuccessful();
        }
    } else {
        QMessageBox::critical(this, "Workspace does not exist",
                              "The workspace does not exist!");
    }
}

void MainWindow::showSaveSuccessful()
{
    QMessageBox::information(nullptr, "Workspace saved",
                             "The workspace was saved successfully!");
}

void MainWindow::showRestoreSuccessful()
{
    QMessageBox::information(nullptr, "Workspace restored",
                             "The workspace was restored successfully!");
}