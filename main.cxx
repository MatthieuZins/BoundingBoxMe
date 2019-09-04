#include "MainWindow.h"
#include <QApplication>

#include <logger.h>

// Global pointer to the mainwindow (used for logging)
MainWindow* mainWindowPtr = nullptr;


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  qInstallMessageHandler(messageHandler);

  MainWindow w;
  mainWindowPtr = &w;
  w.initialize();

  w.show();

  return a.exec();
}
