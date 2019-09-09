#include "displayManager_ui.h"
#include "ui_displayManager_ui.h"

#include "MainWindow.h"

DisplayManager_ui::DisplayManager_ui(QWidget *parent) :
  QGroupBox(parent),
  ui(new Ui::DisplayManager_ui)
{
  ui->setupUi(this);

  QObject::connect(ui->radioButton_Both, SIGNAL(toggled(bool)), this, SLOT(selectModeBoth()));
  QObject::connect(ui->radioButton_Only_Static, SIGNAL(toggled(bool)), this, SLOT(selectModeOnlyStatic()));
  QObject::connect(ui->radioButton_Only_Dynamic, SIGNAL(toggled(bool)), this, SLOT(selectModeOnlyDynamic()));
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
