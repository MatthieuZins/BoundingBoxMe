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
