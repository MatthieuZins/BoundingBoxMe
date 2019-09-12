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
