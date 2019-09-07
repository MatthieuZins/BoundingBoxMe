#include "boundingBoxInformation_ui.h"
#include "ui_boundingBoxInformation_ui.h"

#include <sstream>

#include <Eigen/Dense>

#include "boundingBox.h"
#include "classesManager.h"
#include "timeStepsManager.h"

BoundingBoxInformation_ui::BoundingBoxInformation_ui(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::BoundingBoxInformation_ui)
{
  ui->setupUi(this);
}

BoundingBoxInformation_ui::~BoundingBoxInformation_ui()
{
  delete ui;
}

void BoundingBoxInformation_ui::updateInformation(const BoundingBox *bb)
{
  unsigned int frameToDisplay = TimeStepsManager::getInstance().getFirstOfCurrentFrames();
  ui->label_BB_Id->setText(QString::fromStdString(std::to_string(bb->getInstanceId())));
  ui->comboBox_BB_Class->setCurrentText(QString::fromStdString(bb->getClass()));
  const Eigen::Vector3d& center = bb->getCenter(frameToDisplay);
  std::stringstream ss;
  ss << center.x() << ", " << center.z() << ", " << center.z();
  ui->label_BB_Position->setText(QString::fromStdString(ss.str()));
  ss.str("");
  const Eigen::Quaterniond& orientation = bb->getOrientation(frameToDisplay);
  ss << orientation.w() << ", " << orientation.x() << ", " << orientation.y() << ", " << orientation.z();
  ui->label_BB_Orientation->setText(QString::fromStdString(ss.str()));
  ss.str("");
  auto frames = bb->getFrames();
  for (int i = 0; i < frames.size() - 1;  ++i)
    ss << frames[i] << ", ";
  ss << frames.back();
  ui->label_BB_Frames->setText(QString::fromStdString(ss.str()));
}

void BoundingBoxInformation_ui::updateAvailableClasses(const std::vector<std::string> &availableClasses)
{
  QStringList l;
  for (const auto& c : availableClasses)
  {
    l.append(QString::fromStdString(c));
  }
  ui->comboBox_BB_Class->addItems(l);
}
