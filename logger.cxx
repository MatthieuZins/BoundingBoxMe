//=========================================================================
//
// Copyright 2019 Kitware, Inc.
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

  // Filter out some types of messages
  if (type == QtDebugMsg)
    return;

  if (mainWindowPtr)
    mainWindowPtr->displayLog(out);
}
