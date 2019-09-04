#include "classesManager.h"

#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

std::unique_ptr<ClassesManager> ClassesManager::m_instance = nullptr;


ClassesManager& ClassesManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new ClassesManager);
  return *m_instance;
}

bool ClassesManager::loadFromYaml(const std::string &filename)
{
  try
  {
    YAML::Node file(YAML::LoadFile(filename));
    auto classes = file["classes"];
    for (auto cl : classes)
    {
      std::string name = cl["name"].as<std::string>();
      std::cout << name << "\n";
      Color color;
      color = std::make_tuple(static_cast<unsigned char>(cl["color"][0].as<int>()),     // need to convert first to int because yaml-cpp can't convert to unsigned char
                              static_cast<unsigned char>(cl["color"][1].as<int>()),
                              static_cast<unsigned char>(cl["color"][2].as<int>()));
  //    std::cout << (int)std::get<0>(color) << " " << (int)std::get<1>(color) << " " << (int)std::get<2>(color) << "\n";
      m_map[name] = color;
    }
  } catch (std::exception& e)
  {
    std::cerr << "Error while loading the classes configuration file: " << e.what() << std::endl;
    return false;
  }
  return true;
}

std::vector<std::string> ClassesManager::getclasses() const
{
  std::vector<std::string> classes;
  for (const auto& cl : m_map)
    classes.emplace_back(cl.first);
  return classes;
}

ClassesManager::Color ClassesManager::getClassColor(const std::string &classe) const
{
  if (m_map.find(classe) != m_map.end())
  {
    return m_map.at(classe);
  }
  else
  {
    std::cerr << "Class " << classe << " not found" << std::endl;
    return Color(0, 0, 0);
  }
}
