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

#include "timeStepsManager_ui.h"
#include "ui_timeStepsManager_ui.h"
#include <iostream>

#include "MainWindow.h"

timeStepsManager_ui::timeStepsManager_ui(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::TimeStepsManager_ui)
{
  ui->setupUi(this);

  // By default, "single frame" is selected
  ui->spinBox_intervalFirst->setDisabled(true);
  ui->spinBox_intervalLast->setDisabled(true);

  QObject::connect(ui->radioButton_all, SIGNAL(toggled(bool)), this, SLOT(selectModeAll()));
  QObject::connect(ui->radioButton_single, SIGNAL(toggled(bool)), this, SLOT(selectModeSingle()));
  QObject::connect(ui->radioButton_interval, SIGNAL(toggled(bool)), this, SLOT(selectModeInterval()));


  QObject::connect(ui->spinBox_singleFrame, SIGNAL(valueChanged(int)), this, SLOT(changeSingleStep()));
  QObject::connect(ui->spinBox_intervalFirst, SIGNAL(valueChanged(int)), this, SLOT(changeIntervalBound()));
  QObject::connect(ui->spinBox_intervalLast, SIGNAL(valueChanged(int)), this, SLOT(changeIntervalBound()));

  QObject::connect(ui->pushButton_nextFrame, SIGNAL(clicked(bool)), this, SLOT(nextFrame()));
  QObject::connect(ui->pushButton_previousFrame, SIGNAL(clicked(bool)), this, SLOT(previousFrame()));

}

timeStepsManager_ui::~timeStepsManager_ui()
{
  delete ui;
}

void timeStepsManager_ui::updateTimeStepsBounds(int min, int max)
{
  ui->spinBox_intervalFirst->setMinimum(min);
  ui->spinBox_intervalFirst->setMaximum(max);
  ui->spinBox_intervalLast->setMinimum(min);
  ui->spinBox_intervalLast->setMaximum(max);
  ui->spinBox_singleFrame->setMinimum(min);
  ui->spinBox_singleFrame->setMaximum(max);
}

void timeStepsManager_ui::selectModeAll()
{
  ui->spinBox_intervalFirst->setDisabled(true);
  ui->spinBox_intervalLast->setDisabled(true);
  ui->spinBox_singleFrame->setDisabled(true);
  ui->pushButton_nextFrame->setDisabled(true);
  ui->pushButton_previousFrame->setDisabled(true);
  auto *mainWindow = dynamic_cast<MainWindow*>(window());
  mainWindow->m_timeStepsManager.setModeAll();
  mainWindow->update();
  mainWindow->updateCameraFrameDisplayed();
}

void timeStepsManager_ui::selectModeSingle()
{
  ui->spinBox_intervalFirst->setDisabled(true);
  ui->spinBox_intervalLast->setDisabled(true);
  ui->spinBox_singleFrame->setEnabled(true);
  ui->pushButton_nextFrame->setDisabled(false);
  ui->pushButton_previousFrame->setDisabled(false);

  auto *mainWindow = dynamic_cast<MainWindow*>(window());
  auto val = ui->spinBox_singleFrame->value();
  mainWindow->m_timeStepsManager.setModeSingle(val);
  mainWindow->update();
  mainWindow->updateCameraFrameDisplayed();
}

void timeStepsManager_ui::selectModeInterval()
{
  ui->spinBox_intervalFirst->setEnabled(true);
  ui->spinBox_intervalLast->setEnabled(true);
  ui->spinBox_singleFrame->setDisabled(true);
  ui->pushButton_nextFrame->setDisabled(false);
  ui->pushButton_previousFrame->setDisabled(false);

  auto *mainWindow = dynamic_cast<MainWindow*>(window());
  auto first = ui->spinBox_intervalFirst->value();
  auto last = ui->spinBox_intervalLast->value();
  mainWindow->m_timeStepsManager.setModeInterval(first, last);
  mainWindow->update();
  mainWindow->updateCameraFrameDisplayed();
}

void timeStepsManager_ui::changeSingleStep()
{
  if (ui->radioButton_single->isCheckable())
  {
    // update step
    selectModeSingle();
  }
}

void timeStepsManager_ui::changeIntervalBound()
{
  if (ui->radioButton_interval->isChecked())
  {
    // update steps
    selectModeInterval();
  }
}

void timeStepsManager_ui::nextFrame()
{
  if (ui->radioButton_single->isChecked())
  {
    auto v = ui->spinBox_singleFrame->value();
    ui->spinBox_singleFrame->setValue(v + 1);
  }
  else if (ui->radioButton_interval->isChecked())
  {
    auto first = ui->spinBox_intervalFirst->value();
    auto last = ui->spinBox_intervalLast->value();
    ui->spinBox_intervalFirst->setValue(first + 1);
    ui->spinBox_intervalLast->setValue(last + 1);
  }
}

void timeStepsManager_ui::previousFrame()
{
  if (ui->radioButton_single->isChecked())
  {
    auto v = ui->spinBox_singleFrame->value();
    ui->spinBox_singleFrame->setValue(std::max(0, v - 1));
  }
  else if (ui->radioButton_interval->isChecked())
  {
    auto first = ui->spinBox_intervalFirst->value();
    auto last = ui->spinBox_intervalLast->value();
    ui->spinBox_intervalFirst->setValue(std::max(0, first - 1));
    ui->spinBox_intervalLast->setValue(std::max(0, last - 1));
  }
}
