#ifndef LOGGER_H
#define LOGGER_H

#include <QMessageLogger>
#include <QObject>

class MainWindow;

// global pointer to the main window (defined in main.cxx)
extern MainWindow* mainWindowPtr;


/// Handler for all logging
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOGGER_H
