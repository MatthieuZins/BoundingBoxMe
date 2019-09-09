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
  const BoundingBox *appendBoundingBox(BoundingBox::Id id, const std::string& classe, const Eigen::Isometry3d& pose,
                         const Eigen::Vector3d& dimension, const std::pair<int, int> &framesInterval);

  const BoundingBox* getBoundingBoxFromIndex(unsigned int index) const {
    return m_bbs[index].get();
  }

  BoundingBox* getBoundingBoxFromIndex(unsigned int index) {
    return m_bbs[index].get();
  }

  BoundingBox::Id findFirstUnusedInstanceId() const;

private:
  static std::unique_ptr<BoundingBoxManager> m_instance;

  BoundingBoxManager() = default;
  BoundingBoxManager(const BoundingBoxManager&) = delete;
  BoundingBoxManager& operator =(const BoundingBoxManager&) = delete;

  std::vector<std::shared_ptr<BoundingBox>> m_bbs;

  bool m_initiliazed = false;
  std::unordered_map<std::string, int> m_perClassCount; // just for information purpose

};

#endif // BOUNDINGBOXMANAGER_H
