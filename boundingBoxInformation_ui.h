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
