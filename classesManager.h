#ifndef CLASSESMANAGER_H
#define CLASSESMANAGER_H

#include <memory>
#include <unordered_map>

#include "boundingBox.h"

class ClassesManager
{
  struct Color
  {
    Color(unsigned char red=0, unsigned char green=0, unsigned char blue=0)
      : r(red), g(green), b(blue)
    {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
  };

public:
  static ClassesManager& getInstance();
  ~ClassesManager() = default;

  bool loadFromYaml(const std::string& filename);

  std::vector<std::string> getAvailableClasses() const;

  Color getClassColor(const std::string& classe) const;

  const std::string& getDefaultClass() const {
    return m_defaultClass;
  }

private:
  static std::unique_ptr<ClassesManager> m_instance;

  ClassesManager() = default;
  ClassesManager(const ClassesManager&) = delete;
  ClassesManager& operator =(const ClassesManager&) = delete;

  std::unordered_map<std::string, Color> m_map;
  std::string m_defaultClass;
};

#endif // CLASSESMANAGER_H
