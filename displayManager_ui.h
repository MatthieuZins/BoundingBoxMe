#ifndef DISPLAYMANAGER_UI_H
#define DISPLAYMANAGER_UI_H

#include <QGroupBox>

namespace Ui {
class DisplayManager_ui;
}

class DisplayManager_ui : public QGroupBox
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
