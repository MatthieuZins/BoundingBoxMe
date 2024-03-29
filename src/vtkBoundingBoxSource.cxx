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

#include "vtkBoundingBoxSource.h"

#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkPoints.h>

vtkStandardNewMacro(vtkBoundingBoxSource);


int vtkBoundingBoxSource::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{

  this->Superclass::RequestData(request, inputVector, outputVector);

  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Add a "mark" to indicate the front of the BBox
  auto pts = output->GetPoints();
  int nbPoints = output->GetNumberOfPoints();
  double size = 0.02;
  double shiftZ = 0.25;

  pts->InsertNextPoint(-size, 0.5, -size + shiftZ);
  pts->InsertNextPoint(-size, 0.5,  size + shiftZ);
  pts->InsertNextPoint( size, 0.5, -size + shiftZ);
  pts->InsertNextPoint( size, 0.5,  size + shiftZ);

  pts->InsertNextPoint(-size, 1.0, -size + shiftZ);
  pts->InsertNextPoint(-size, 1.0,  size + shiftZ);
  pts->InsertNextPoint( size, 1.0, -size + shiftZ);
  pts->InsertNextPoint( size, 1.0,  size + shiftZ);

  vtkIdType facePts1[4] = {nbPoints+0, nbPoints+1, nbPoints+5, nbPoints+4};
  vtkIdType facePts2[4] = {nbPoints+0, nbPoints+2, nbPoints+3, nbPoints+1};
  vtkIdType facePts3[4] = {nbPoints+2, nbPoints+6, nbPoints+7, nbPoints+3};
  vtkIdType facePts4[4] = {nbPoints+4, nbPoints+5, nbPoints+7, nbPoints+6};
  vtkIdType facePts5[4] = {nbPoints+1, nbPoints+3, nbPoints+7, nbPoints+5};
  vtkIdType facePts6[4] = {nbPoints+0, nbPoints+4, nbPoints+6, nbPoints+2};

  auto faces = output->GetPolys();
  faces->InsertNextCell(4, facePts1);
  faces->InsertNextCell(4, facePts2);
  faces->InsertNextCell(4, facePts3);
  faces->InsertNextCell(4, facePts4);
  faces->InsertNextCell(4, facePts5);
  faces->InsertNextCell(4, facePts6);

  return 1;
}
