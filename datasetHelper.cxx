#include "datasetHelper.h"

#include <fstream>
#include <sstream>

#include <QDebug>
#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <lidarFrameManager.h>
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include "vtkEigenUtils.h"

#include <unsupported/Eigen/EulerAngles>

namespace
{
vtkDoubleArray* foundArray(vtkTable* table, std::vector<std::string> potentialName)
{
  // try to find the right array
  for (const auto& name: potentialName)
  {
    vtkDoubleArray* tmp = vtkDoubleArray::SafeDownCast(table->GetColumnByName(name.c_str()));
    if (tmp)
    {
      return tmp;
    }
  }
  // throw an exception when no matching could be founded
  std::stringstream ss;
  ss << "Could not find of the following colum: ";
  for (const auto& name: potentialName)
  {
    ss << name << " , ";
  }
  throw std::runtime_error(ss.str());
}

std::unordered_map<std::string, vtkDoubleArray*> createArrayIndex(vtkTable* table)
{
  std::unordered_map<std::string, vtkDoubleArray*> array;
  std::vector<std::string> potentialName;

  std::string time[] = {"Time", "time", "Timestamp", "timestamp"};
  potentialName = std::vector<std::string>(time, time + sizeof(time) / sizeof(time[0]));
  array["time"] = foundArray(table, potentialName);

  std::string roll[] = {"Rx(Roll)", "Roll", "roll", "Rx", "rx"};
  potentialName = std::vector<std::string>(roll, roll + sizeof(roll) / sizeof(roll[0]));
  array["roll"] = foundArray(table, potentialName);

  std::string pitch[] = {"Ry(Pitch)", "Pitch", "pitch", "Ry", "ry"};
  potentialName = std::vector<std::string>(pitch, pitch + sizeof(pitch) / sizeof(pitch[0]));
  array["pitch"] = foundArray(table, potentialName);

  std::string yaw[] = {"Rz(Yaw)", "Yaw", "yaw", "Rz", "rz"};
  potentialName = std::vector<std::string>(yaw, yaw + sizeof(yaw) / sizeof(yaw[0]));
  array["yaw"] = foundArray(table, potentialName);

  std::string x[] = {"X", "x"};
  potentialName = std::vector<std::string>(x, x + sizeof(x) / sizeof(x[0]));
  array["X"] = foundArray(table, potentialName);

  std::string y[] = {"Y", "y"};
  potentialName = std::vector<std::string>(y, y + sizeof(y) / sizeof(y[0]));
  array["Y"] = foundArray(table, potentialName);

  std::string z[] = {"Z", "z"};
  potentialName = std::vector<std::string>(z, z + sizeof(z) / sizeof(z[0]));
  array["Z"] = foundArray(table, potentialName);

  return array;
}
}


std::vector<std::pair<std::string, double> > loadSeriesFile(const std::string &filename)
{
  std::vector<std::pair<std::string, double>> dataset;
  try
  {
    YAML::Node node = YAML::LoadFile(filename);
    YAML::Node files = node["files"];
    fs::path directory = fs::path(filename).parent_path();

    for (size_t i = 0; i < files.size(); ++i)
    {
      fs::path file(files[i]["name"].as<std::string>());

      dataset.emplace_back(std::make_pair((directory / file).generic_string(), files[i]["time"].as<double>()));
    }
  }
  catch (std::exception& e)
  {
    qCritical() << "Error opening the dataset: " << QString::fromStdString(e.what());
  }
  return dataset;
}


std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d> > loadPosesFile(const std::string &filename)
{
  std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> poses;
  std::ifstream file(filename);
  if (file.is_open())
  {
    // Read the data from the file
    vtkNew<vtkDelimitedTextReader> csvReader;
    csvReader->SetFileName(filename.c_str());
    csvReader->DetectNumericColumnsOn();
    csvReader->SetFieldDelimiterCharacters(",");
    csvReader->ForceDoubleOn();
    csvReader->SetHaveHeaders(true);
    csvReader->Update();

    vtkTable* table = csvReader->GetOutput();

    if (table->GetNumberOfColumns() < 7)
    {
      qCritical() << "Invalid poses file: incorrect number of columns";
      return poses;
    }

    // create map for idx
    std::unordered_map<std::string, vtkDoubleArray*> array;
    try
    {
      array = createArrayIndex(table);
    }
    catch (std::exception& e)
    {
      qCritical() << "Invalid poses file: " << e.what();
      return poses;
    }

    // Read the data from the csv table
    for (vtkIdType i = 0; i < table->GetNumberOfRows(); i++)
    {
     Eigen::Translation3d t(array["X"]->GetTuple1(i), array["Y"]->GetTuple1(i), array["Z"]->GetTuple1(i));
     Eigen::EulerAnglesXYZd r(array["roll"]->GetTuple1(i), array["pitch"]->GetTuple1(i), array["yaw"]->GetTuple1(i));
     poses.emplace_back(t *  Eigen::Quaterniond(r.toRotationMatrix()));
    }
  }
  else
  {
    qCritical() << "Impossible to open the poses file: " << QString::fromStdString(filename);
  }
  return poses;
}


bool loadDataSet(const std::string &filename)
{
  fs::path path(filename);
  fs::path directory = path.parent_path();

  fs::path name = path.filename();
  fs::path posesFile = directory / name.replace_extension("poses");

  std::cout << "Load DataSet\n";
  std::cout << "Series file: " << filename << "\n";
  std::cout << "Poses file: " << posesFile.generic_string() << "\n";


  auto listOfFramesAndTimes = loadSeriesFile(filename);
  if (listOfFramesAndTimes.size() > 0)
  {
    auto poses = loadPosesFile(posesFile.generic_string());

    if (poses.size() != listOfFramesAndTimes.size())
    {
      qCritical() << "Erro opening the dataset: number of .vtp is different from number of poses";
    }

    auto& lidarFramesManager = LidarFrameManager::getInstance();

    auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    auto transform = vtkSmartPointer<vtkTransform>::New();

    for (size_t i = 0; i < listOfFramesAndTimes.size(); ++i)
    {
      reader->SetFileName(listOfFramesAndTimes[i].first.c_str());

      transform->SetMatrix(eigenIsometry3dToVtkMatrix4x4(poses[i]));
      transformFilter->SetInputConnection(reader->GetOutputPort());
      transformFilter->SetTransform(transform);
      transformFilter->Update();
      lidarFramesManager.addFrame({i, transformFilter->GetOutput(), poses[i], listOfFramesAndTimes[i].second});
      vtkPolyData* pd = transformFilter->GetOutput();
      pd->GetPointData()->SetActiveScalars("intensity");
    }
    return true;
  }
  return false;
}
