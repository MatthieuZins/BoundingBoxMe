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

#include "MainWindow.h"
#include <QApplication>

#include "logger.h"

// Global pointer to the mainwindow (used for logging)
MainWindow* mainWindowPtr = nullptr;


int main(int argc, char *argv[])
{
  int exitCode = RESTART_CODE;
  while (exitCode == RESTART_CODE)
  {
    mainWindowPtr = nullptr;
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
