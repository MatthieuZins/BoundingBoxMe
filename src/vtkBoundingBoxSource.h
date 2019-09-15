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

#ifndef VTKBOUNDINGBOXSOURCE_H
#define VTKBOUNDINGBOXSOURCE_H

#include "vtkFiltersSourcesModule.h" // For export macro
#include <vtkCubeSource.h>

/// This class specializes the vtkCubeSource to also display the front of the BBox
class VTKFILTERSSOURCES_EXPORT vtkBoundingBoxSource : public vtkCubeSource
{
public:
  static vtkBoundingBoxSource *New();
  vtkTypeMacro(vtkBoundingBoxSource, vtkCubeSource);



protected:
  vtkBoundingBoxSource(double xL=1.0, double yL=1.0, double zL=1.0)
    : vtkCubeSource(xL, yL, zL)
  {}
  ~vtkBoundingBoxSource() = default;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;


  private:
    vtkBoundingBoxSource(const vtkBoundingBoxSource&) = delete;
    void operator=(const vtkBoundingBoxSource&) = delete;
};


#endif // VTKBOUNDINGBOXSOURCE_H
