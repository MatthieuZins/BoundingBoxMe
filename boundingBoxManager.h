#ifndef BOUNDINGBOXMANAGER_H
#define BOUNDINGBOXMANAGER_H

#include <memory>
#include <unordered_map>

#include "classesManager.h"

class BoundingBoxManager
{

public:
  static BoundingBoxManager& getInstance();
  ~BoundingBoxManager() = default;


  /// Iinitializes the classes to handle from the classes manager
  /// This is called only once
  void initializeClassesToHandle(const ClassesManager& classesManager);

  // for now test method
  const BoundingBox *addBoundingBox(BoundingBox::Id id, const std::string& classe,
                                    const Eigen::Isometry3d& pose, const Eigen::Vector3d& dimension,
                                    const std::pair<int, int> &framesInterval);

  const BoundingBox* getBoundingBoxFromIndex(unsigned int index) const {
    return m_bbs[index].get();
  }

  BoundingBox* getBoundingBoxFromIndex(unsigned int index) {
    return m_bbs[index].get();
  }

  BoundingBox::Id findFirstUnusedInstanceId() const;

  BoundingBox* getBoundingBoxFromInstanceId(BoundingBox::Id id);

  bool deleteBoundingBox(unsigned int index, const std::pair<int, int> &framesInterval);

  void updateBoundingBoxInstanceId(BoundingBox* bb, unsigned int id);


private:
  static std::unique_ptr<BoundingBoxManager> m_instance;

  BoundingBoxManager() = default;
  BoundingBoxManager(const BoundingBoxManager&) = delete;
  BoundingBoxManager& operator =(const BoundingBoxManager&) = delete;

  // We store the bbs in a vector which gets resized when we add or delete a bb
  // this vector and the vtkActor, ... from mainWindow have a 1-to-1 correspondance
  // each bounding box stores its index in storingId

  // maybe a vector with a max size should be prefered
  // storing id and instance id are completeley unlinked, so we could avoid to erase a boundingbox
  // and just leave it empty
  std::vector<std::shared_ptr<BoundingBox>> m_bbs;

  bool m_initiliazed = false;
  std::unordered_map<std::string, int> m_perClassCount; // just for information purpose

};

#endif // BOUNDINGBOXMANAGER_H
