#include "boundingBoxManager.h"

std::unique_ptr<BoundingBoxManager> BoundingBoxManager::m_instance = nullptr;


BoundingBoxManager& BoundingBoxManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new BoundingBoxManager);
  return *m_instance;
}

void BoundingBoxManager::initializeClassesToHandle(const ClassesManager &classesManager)
{
  if (!m_initiliazde)
  {

    for (const auto& cl : classesManager.getclasses())
    {
      m_perClassCount[cl] = 0;
    }
  }
}
