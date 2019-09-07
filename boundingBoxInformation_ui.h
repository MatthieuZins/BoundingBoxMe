#ifndef BOUNDINGBOXINFORMATION_UI_H
#define BOUNDINGBOXINFORMATION_UI_H

#include <QWidget>

class BoundingBox;

namespace Ui {
class BoundingBoxInformation_ui;
}

class BoundingBoxInformation_ui : public QWidget
{
  Q_OBJECT

public:
  explicit BoundingBoxInformation_ui(QWidget *parent = nullptr);
  ~BoundingBoxInformation_ui();

  void updateInformation(const BoundingBox *bb);
  void updateAvailableClasses(const std::vector<std::string>& availableClasses);


private:
  Ui::BoundingBoxInformation_ui *ui;
};

#endif // BOUNDINGBOXINFORMATION_UI_H
