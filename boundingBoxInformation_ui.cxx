#include "boundingBoxInformation_ui.h"
#include "ui_boundingBoxInformation_ui.h"

#include <sstream>

#include <Eigen/Dense>

#include "boundingBox.h"
#include "classesManager.h"
#include "timeStepsManager.h"
#include "MainWindow.h"

BoundingBoxInformation_ui::BoundingBoxInformation_ui(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::BoundingBoxInformation_ui)
{
  ui->setupUi(this);

  QObject::connect(ui->comboBox_BB_Class, SIGNAL(activated(int)), this, SLOT(updateBoundingBoxClass(int)));

}

BoundingBoxInformation_ui::~BoundingBoxInformation_ui()
{
  delete ui;
}

void BoundingBoxInformation_ui::updateInformation(BoundingBox *bb)
{
  m_bb = bb;
  unsigned int frameToDisplay = TimeStepsManager::getInstance().getCurrentTimeInterval().first;
  ui->label_BB_Id->setText(QString::fromStdString(std::to_string(bb->getInstanceId())));
  ui->comboBox_BB_Class->setCurrentText(QString::fromStdString(bb->getClass()));
  const Eigen::Vector3d& center = bb->getCenter(frameToDisplay);
  std::stringstream ss;
  std::cout << "update cneter = " << center.transpose() << std::endl;
  ss << center.x() << ", " << center.y() << ", " << center.z();
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

void BoundingBoxInformation_ui::unselectBoundingBox() {
  m_bb = nullptr;
  ui->label_BB_Id->setText(QString());
  ui->label_BB_Position->setText(QString());
  ui->label_BB_Orientation->setText(QString());
  ui->label_BB_Frames->setText(QString());
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
    }
  }
}
