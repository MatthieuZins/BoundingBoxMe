#include "logger.h"

#include <QDebug>
#include <QDateTime>

#include "MainWindow.h"


void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  QString out;
  out.append("<span style=\" font-size:8pt; font-weight:600; color:");
  switch (type)
  {
    case QtInfoMsg:     out.append("#000000 "); break;
    case QtDebugMsg:    out.append("#00aa00 "); break;
    case QtWarningMsg:  out.append("#f58d42 "); break;
    case QtCriticalMsg: out.append("#ff0c32 "); break;
    case QtFatalMsg:    out.append("#ff0c32 "); break;
  }
  out.append(";\" > ");

  out.append(QDateTime::currentDateTime().toString("hh:mm:ss.zzz "));

  switch (type)
  {
    case QtInfoMsg:     out.append("Info: "); break;
    case QtDebugMsg:    out.append("Debug: "); break;
    case QtWarningMsg:  out.append("Warning: "); break;
    case QtCriticalMsg: out.append("Critical: "); break;
    case QtFatalMsg:    out.append("Fatal: "); break;
  }
  out.append(msg);
  out.append("</span>");

  if (mainWindowPtr)
    mainWindowPtr->displayLog(out);
}
