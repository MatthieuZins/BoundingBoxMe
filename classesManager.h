#ifndef CLASSESMANAGER_H
#define CLASSESMANAGER_H

#include <memory>
#include <unordered_map>

#include "boundingBox.h"

class ClassesManager
{
  using Color = std::tuple<unsigned char, unsigned char, unsigned char>;

public:
  static ClassesManager& getInstance();
  ~ClassesManager() = default;

  bool loadFromYaml(const std::string& filename);

  std::vector<std::string> getclasses() const;

  Color getClassColor(const std::string& classe) const;

private:
  static std::unique_ptr<ClassesManager> m_instance;

  ClassesManager() = default;
  ClassesManager(const ClassesManager&) = delete;
  ClassesManager& operator =(const ClassesManager&) = delete;

  std::unordered_map<std::string, Color> m_map;
};

#endif // CLASSESMANAGER_H
