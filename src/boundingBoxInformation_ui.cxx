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

#include "boundingBoxInformation_ui.h"
#include "ui_boundingBoxInformation_ui.h"

#include <sstream>

#include <Eigen/Dense>

#include "boundingBox.h"
#include "classesManager.h"
#include "timeStepsManager.h"
#include "MainWindow.h"

BoundingBoxInformation_ui::BoundingBoxInformation_ui(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::BoundingBoxInformation_ui)
{
  ui->setupUi(this);

  QObject::connect(ui->comboBox_BB_Class, SIGNAL(activated(int)), this, SLOT(updateBoundingBoxClass(int)));
  QObject::connect(ui->comboBox_BB_Id, SIGNAL(activated(int)), this, SLOT(updateBoundingBoxInstanceId(int)));

}

BoundingBoxInformation_ui::~BoundingBoxInformation_ui()
{
  delete ui;
}

void BoundingBoxInformation_ui::updateInformation(BoundingBox *bb)
{
  m_bb = bb;
  auto [firstFrame, lastFrame] = TimeStepsManager::getInstance().getCurrentTimeInterval();
  unsigned int frameToDisplay = bb->getFirstFrameOfPresenceInInterval(firstFrame, lastFrame);

  ui->comboBox_BB_Id->setCurrentText(QString::number(bb->getInstanceId()));
  ui->comboBox_BB_Class->setCurrentText(QString::fromStdString(bb->getClass()));
  std::stringstream ss;
  int space = 5;
  ss << std::setprecision(2);
  ss.setf(std::ios::fixed);

  const Eigen::Vector3d& center = bb->getCenter(frameToDisplay);
  ss << std::setw(space) << center.x() << ", " << std::setw(space) << center.y() << ", "
     << std::setw(space) << center.z();
  ui->label_BB_Position->setText(QString::fromStdString(ss.str()));
  ss.str("");

  const Eigen::Quaterniond& orientation = bb->getOrientation(frameToDisplay);
  ss << std::setw(space) << orientation.w() << ", " << std::setw(space) << orientation.x() << ", "
     << std::setw(space) << orientation.y() << ", " << std::setw(space) << orientation.z();
  ui->label_BB_Orientation->setText(QString::fromStdString(ss.str()));
  ss.str("");

  const Eigen::Vector3d& dimensions = bb->getDimensions();
  ss << std::setw(space) << dimensions.x() << ", " << std::setw(space) << dimensions.y() << ", "
     << std::setw(space) << dimensions.z();
  ui->label_BB_Dimensions->setText(QString::fromStdString(ss.str()));
  ss.str("");

  auto frames = bb->getFrames();
  if (frames.size() <= 5)
  {
    for (int i = 0; i < frames.size() - 1;  ++i)
      ss << frames[i] << ", ";
    ss << frames.back();
  }
  else  // limit the number of frame ids to display
  {
    ss << frames[0] << ",  "
       <<frames[1] << ", ..., "
       << frames[frames.size()-2] << ", "
       << frames.back();
  }
  ui->label_BB_Frames->setText(QString::fromStdString(ss.str()));

  auto state = bb->getState();
  if (state == BoundingBox::State::STATIC)
    ui->label_BB_State->setText("Static");
  else if (state == BoundingBox::State::DYNAMIC)
    ui->label_BB_State->setText("Dynamic");

}

void BoundingBoxInformation_ui::updateAvailableClasses(const std::vector<std::string> &availableClasses)
{
  ui->comboBox_BB_Class->clear();
  QStringList l;
  for (const auto& c : availableClasses)
  {
    l.append(QString::fromStdString(c));
  }
  ui->comboBox_BB_Class->addItems(l);
}

void BoundingBoxInformation_ui::updateAvailableInstanceIds(const std::vector<unsigned int> &availableIds)
{
  ui->comboBox_BB_Id->clear();
  QStringList l;
  for (const auto& id : availableIds)
  {
    l.append(QString::number(id));
  }
  ui->comboBox_BB_Id->addItems(l);
}

void BoundingBoxInformation_ui::unselectBoundingBox()
{
  m_bb = nullptr;
  ui->comboBox_BB_Id->clear();
  ui->label_BB_Position->setText(QString());
  ui->label_BB_Orientation->setText(QString());
  ui->label_BB_Frames->setText(QString());
  ui->label_BB_Dimensions->setText(QString());
  ui->label_BB_State->setText(QString());
}

std::string BoundingBoxInformation_ui::getCurrentlySelectedClass()
{
  return ui->comboBox_BB_Class->currentText().toStdString();
}

void BoundingBoxInformation_ui::updateBoundingBoxClass(int index)
{
  const auto& classe = ui->comboBox_BB_Class->itemText(index);
  if (m_bb)
  {
    m_bb->setClass(classe.toStdString());
    updateInformation(m_bb);
    auto *mainWindow = dynamic_cast<MainWindow*>(window());
    if (mainWindow)
    {
      mainWindow->editBoundingBox(m_bb->getStoringId());
      mainWindow->forceRender();
    }
  }
}

void BoundingBoxInformation_ui::updateBoundingBoxInstanceId(int index)
{
  const auto& instanceIdStr = ui->comboBox_BB_Id->itemText(index);
  if (m_bb)
  {
    auto *mainWindow = dynamic_cast<MainWindow*>(window());
    if (mainWindow)
    {
      mainWindow->updateBoundingBoxInstanceId(m_bb->getStoringId(), instanceIdStr.toUInt());
    }
  }
}
