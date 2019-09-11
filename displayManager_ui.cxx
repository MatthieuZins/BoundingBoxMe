#include "displayManager_ui.h"
#include "ui_displayManager_ui.h"

#include <sstream>

#include "MainWindow.h"

DisplayManager_ui::DisplayManager_ui(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DisplayManager_ui)
{
  ui->setupUi(this);

  QObject::connect(ui->radioButton_Both, SIGNAL(toggled(bool)), this, SLOT(selectModeBoth()));
  QObject::connect(ui->radioButton_Only_Static, SIGNAL(toggled(bool)), this, SLOT(selectModeOnlyStatic()));
  QObject::connect(ui->radioButton_Only_Dynamic, SIGNAL(toggled(bool)), this, SLOT(selectModeOnlyDynamic()));

  ui->horizontalSlider_Percentage->setMinimum(1);
  ui->horizontalSlider_Percentage->setMaximum(4);
  ui->horizontalSlider_Percentage->setValue(4);

  QObject::connect(ui->horizontalSlider_Percentage, SIGNAL(valueChanged(int)), this, SLOT(updateSkipMode(int)));

  ui->spinBox_Threshold->setMaximum(std::numeric_limits<int>::max());
  ui->spinBox_Threshold->setMinimum(1500);
  ui->spinBox_Threshold->setValue(1500);

  QObject::connect(ui->spinBox_Threshold, SIGNAL(valueChanged(int)), this, SLOT(updateThreshold(int)));

}

DisplayManager_ui::~DisplayManager_ui()
{
  delete ui;
}

void DisplayManager_ui::selectModeBoth()
{
  m_displayMode = DisplayMode::BOTH;
  static_cast<MainWindow*>(window())->update();
}

void DisplayManager_ui::selectModeOnlyStatic()
{
  m_displayMode = DisplayMode::ONLY_STATIC;
  static_cast<MainWindow*>(window())->update();
}

void DisplayManager_ui::selectModeOnlyDynamic()
{
  m_displayMode = DisplayMode::ONLY_DYNAMIC;
  static_cast<MainWindow*>(window())->update();
}

void DisplayManager_ui::updateSkipMode(int v)
{
  double percentage = m_precentages[v-1];
  std::stringstream ss;
  ss << std::setprecision(2) << percentage;
  ui->label_Percentage->setText(QString::fromStdString(ss.str()));
  auto *mainWindow = dynamic_cast<MainWindow*>(window());
  mainWindow->setSkipFramesMode(v);
  mainWindow->update();
}

void DisplayManager_ui::updateThreshold(int v)
{
  auto *mainWindow = dynamic_cast<MainWindow*>(window());
  mainWindow->setFastRenderingThreshold(v);
  mainWindow->update();
}
