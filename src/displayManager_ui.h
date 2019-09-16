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

#ifndef DISPLAYMANAGER_UI_H
#define DISPLAYMANAGER_UI_H

#include <QWidget>

namespace Ui {
class DisplayManager_ui;
}

class DisplayManager_ui : public QWidget
{
  Q_OBJECT


public:

  enum class DisplayMode {
    BOTH,
    ONLY_STATIC,
    ONLY_DYNAMIC
  };

  explicit DisplayManager_ui(QWidget *parent = nullptr);
  ~DisplayManager_ui();

  DisplayMode getDisplayMode() const {
    return m_displayMode;
  }

public slots:
  void selectModeBoth();
  void selectModeOnlyStatic();
  void selectModeOnlyDynamic();
  void updateSkipMode(int v);
  void updateThreshold(int v);

private:
  Ui::DisplayManager_ui *ui;

  DisplayMode m_displayMode = DisplayMode::BOTH;

  std::vector<double> m_precentages = {0.25, 0.5, 0.75, 1.0};


};

#endif // DISPLAYMANAGER_UI_H
