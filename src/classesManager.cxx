//=========================================================================
//
// Copyright 2019 Kitware, Inc.
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "classesManager.h"

#include <iostream>
#include <string>

#include <QDebug>
#include <yaml-cpp/yaml.h>

#include "logger.h"

std::unique_ptr<ClassesManager> ClassesManager::m_instance = nullptr;


ClassesManager& ClassesManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new ClassesManager);
  return *m_instance;
}

void ClassesManager::releaseInstance()
{
  if (m_instance)
    m_instance.reset();
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
      Color color;
      color.r = static_cast<unsigned char>(cl["color"][0].as<int>());     // need to convert first to int because yaml-cpp can't convert to unsigned char
      color.g = static_cast<unsigned char>(cl["color"][1].as<int>());
      color.b = static_cast<unsigned char>(cl["color"][2].as<int>());
      Colord colord(color);
      m_map[name] = color;
      m_mapd[name] = colord;
    }
  } catch (std::exception& e)
  {
    return false;
  }
  return true;
}

std::vector<std::string> ClassesManager::getAvailableClasses() const
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
    qWarning() << "Class " << QString::fromStdString(classe) << " not found";
    return Color(0, 0, 0);
  }
}


ClassesManager::Colord ClassesManager::getClassColord(const std::string &classe) const
{
  if (m_map.find(classe) != m_map.end())
  {
    return m_mapd.at(classe);
  }
  else
  {
    qWarning() << "Class " << QString::fromStdString(classe) << " not found";
    return Colord(Color(0, 0, 0));
  }
}
