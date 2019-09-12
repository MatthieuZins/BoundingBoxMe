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


  struct Colord
  {
    Colord(double red=0.0, double green=0.0, double blue=0.0)
      : r(red), g(green), b(blue)
    {}

    Colord(const Color& color)
      : r(static_cast<float>(color.r) / 255),
        g(static_cast<float>(color.g) / 255),
        b(static_cast<float>(color.b) / 255)
    {}

    double r;
    double g;
    double b;
  };

public:
  static ClassesManager& getInstance();
  static void releaseInstance();
  ~ClassesManager() = default;

  bool loadFromYaml(const std::string& filename);

  std::vector<std::string> getAvailableClasses() const;

  Color getClassColor(const std::string& classe) const;
  Colord getClassColord(const std::string& classe) const;

private:
  static std::unique_ptr<ClassesManager> m_instance;

  ClassesManager() = default;
  ClassesManager(const ClassesManager&) = delete;
  ClassesManager& operator =(const ClassesManager&) = delete;

  /// This class stores both the unsigned char and the double versions of the color.
  /// This is not a problem because the number of classe remains "small".
  /// And this enable us to avoid any conversion inside other processing loops.
  /// You just need to call the right method.
  std::unordered_map<std::string, Color> m_map;
  std::unordered_map<std::string, Colord> m_mapd;
};

#endif // CLASSESMANAGER_H
