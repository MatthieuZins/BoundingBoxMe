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

#ifndef TIMESTEPSMANAGER_UI_H
#define TIMESTEPSMANAGER_UI_H

#include <QGroupBox>

namespace Ui {
class TimeStepsManager_ui;
}

class timeStepsManager_ui : public QGroupBox
{
  Q_OBJECT

public:
  explicit timeStepsManager_ui(QWidget *parent = nullptr);
  ~timeStepsManager_ui();

  void updateTimeStepsBounds(int min, int max);

public slots:
  void selectModeAll();
  void selectModeSingle();
  void selectModeInterval();

  void changeSingleStep();
  void changeIntervalBound();

  void nextFrame();
  void previousFrame();

private:
  Ui::TimeStepsManager_ui *ui;


};

#endif // TIMESTEPSMANAGER_UI_H
