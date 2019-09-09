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

private:
  Ui::DisplayManager_ui *ui;

  DisplayMode m_displayMode = DisplayMode::BOTH;
};

#endif // DISPLAYMANAGER_UI_H
