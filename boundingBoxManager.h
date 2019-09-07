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
  std::shared_ptr<BoundingBox> appendBoundingBox(BoundingBox::Id id, const std::string& classe, const Eigen::Isometry3d& pose,
                         const Eigen::Vector3d& dimension, int frame);

  const BoundingBox* getBoundingBoxFromIndex(unsigned int index) const {
    return m_bbs[index].get();
  }

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
