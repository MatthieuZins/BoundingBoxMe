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

#ifndef BOUNDINGBOXINFORMATION_UI_H
#define BOUNDINGBOXINFORMATION_UI_H

#include <QGroupBox>

class BoundingBox;

namespace Ui {
class BoundingBoxInformation_ui;
}

class BoundingBoxInformation_ui : public QGroupBox
{
  Q_OBJECT

public:
  explicit BoundingBoxInformation_ui(QWidget *parent = nullptr);
  ~BoundingBoxInformation_ui();

  void updateInformation(BoundingBox *bb);

  void updateAvailableClasses(const std::vector<std::string>& availableClasses);
  void updateAvailableInstanceIds(const std::vector<unsigned int>& availableIds);

  void unselectBoundingBox();

  std::string getCurrentlySelectedClass();

public slots:
  void updateBoundingBoxClass(int index);
  void updateBoundingBoxInstanceId(int index);

private:
  Ui::BoundingBoxInformation_ui *ui;
  BoundingBox *m_bb = nullptr;
};

#endif // BOUNDINGBOXINFORMATION_UI_H
