#ifndef BOUNDINGBOXMANAGER_H
#define BOUNDINGBOXMANAGER_H

#include <memory>

#include "classesManager.h"


class BoundingBoxManager
{

public:
  static BoundingBoxManager& getInstance();
  ~BoundingBoxManager() = default;



  /// Adds a new bounding box
  /// If the bounding box with this instance id already exists, the new presence are added
  /// Otherwise a new bounding box is created
  /// Returns a pointer to the new bounding box
  const BoundingBox *addBoundingBox(BoundingBox::Id id, const std::string& classe,
                                    const Eigen::Isometry3d& pose, const Eigen::Vector3d& dimension,
                                    const std::pair<int, int> &framesInterval);


  /// Get a const pointer to a bounding box from its index (or storing id)
  const BoundingBox* getBoundingBoxFromIndex(unsigned int index) const;

  /// Get a pointer to a bounding box from its index (or storing id)
  BoundingBox* getBoundingBoxFromIndex(unsigned int index);

  /// Find the lowest unused instance id
  BoundingBox::Id findFirstUnusedInstanceId() const;

  /// Find a bounding box from its instance id
  BoundingBox* getBoundingBoxFromInstanceId(BoundingBox::Id id);

  /// Delete the presence of a bounding box stored at index between framesInterval
  ///  return true if the bounding box gets completely deleted because it has no presence anymore
  ///  return false if the bounding box is still present in some other frames
  bool deleteBoundingBox(unsigned int index, const std::pair<int, int> &framesInterval);


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
};

inline const BoundingBox *BoundingBoxManager::getBoundingBoxFromIndex(unsigned int index) const
{
  return m_bbs[index].get();
}

inline BoundingBox *BoundingBoxManager::getBoundingBoxFromIndex(unsigned int index)
{
  return m_bbs[index].get();
}

#endif // BOUNDINGBOXMANAGER_H
