#include "MainWindow.h"
#include <QApplication>

#include <logger.h>

// Global pointer to the mainwindow (used for logging)
MainWindow* mainWindowPtr = nullptr;


int main(int argc, char *argv[])
{
  int exitCode = RESTART_CODE;
  while (exitCode == RESTART_CODE)
  {
    QApplication a(argc, argv);
    qInstallMessageHandler(messageHandler);
    MainWindow w;
    mainWindowPtr = &w;
    w.initialize();

    w.show();

    exitCode = a.exec();
  }
  return exitCode;
}
