/*
 * VTKDemos
 * Copyright (C) 2013 Juan Hernando jhernando@fi.upm.es
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "common/paths.h"

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkPointSource.h>

vtkSmartPointer<vtkPolyDataMapper> pointGrid();
vtkSmartPointer<vtkPolyDataMapper> pointSource();

int main()
{
  // Create the geometry of a point (the coordinate)
  vtkSmartPointer<vtkPoints> points =
    vtkSmartPointer<vtkPoints>::New();
  const float p[3] = {1.0, 2.0, 3.0};

  // Create the topology of the point (a vertex)
  vtkSmartPointer<vtkCellArray> vertices =
    vtkSmartPointer<vtkCellArray>::New();
  vtkIdType pid[1];
  pid[0] = points->InsertNextPoint(p);
  vertices->InsertNextCell(1,pid);

  // Create a polydata object
  vtkSmartPointer<vtkPolyData> point =
    vtkSmartPointer<vtkPolyData>::New();

  // Set the points and vertices we created as the geometry and topology of the polydata
  point->SetPoints(points);
  point->SetVerts(vertices);

  // Visualize
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInput(point);
#else
  mapper->SetInputData(point);
#endif

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(20);

  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);

  renderWindow->Render();
  renderWindowInteractor->Start();
//
//    vtkSmartPointer<vtkPolyDataMapper> mapper = pointGrid();
//    //vtkSmartPointer<vtkPolyDataMapper> mapper = pointSource();
//
//    /* Assinging the mapper to an actor */
//    vtkSmartPointer<vtkActor> actor = vtkActor::New();
//    actor->SetMapper(mapper);
//    actor->GetProperty()->SetColor(1, 0, 0);
//
//    /* Creating the renderer and the render window */
//    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
//    renderer->AddActor(actor);
//    renderer->SetBackground(0.2, 0.3, 0.4);
//
//    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
//    window->AddRenderer(renderer);
//    window->SetSize(800, 800);
//
//    /* Creating the interactor that handles the window events and provides
//       the main rendering loop */
//    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
//        vtkRenderWindowInteractor::New();
//    interactor->SetRenderWindow(window);
//    interactor->Initialize();
//    interactor->Start();
}

vtkSmartPointer<vtkPolyDataMapper> pointGrid()
{
    /* Creating a poly data object of 10x10x10 points */
    vtkSmartPointer<vtkPolyData> data = vtkPolyData::New();
    size_t width = 10;
    size_t height = 10;
    size_t depth = 10;
    vtkSmartPointer<vtkPoints> vertices = vtkPoints::New();
    for (size_t i = 0; i < width; ++i)
        for (size_t j = 0; j < height; ++j)
            for (size_t k = 0; k < depth; ++k)
                vertices->InsertPoint(i * height * depth + j * depth + k,
                                      i * 10.0, j * 10.0, k * 10.0);
    data->SetPoints(vertices);

    vtkSmartPointer<vtkCellArray> points = vtkCellArray::New();
    for (vtkIdType i = 0; i < vtkIdType(width * height * depth); ++i)
        points->InsertNextCell(0, &i);
    data->SetVerts(points);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    /* Assigning the data object directly to the poly mapper */
    mapper->SetInputDataObject(data);
    return mapper;
}

vtkSmartPointer<vtkPolyDataMapper> pointSource()
{
    vtkSmartPointer<vtkPointSource> points =
        vtkSmartPointer<vtkPointSource>::New();
    points->SetNumberOfPoints(100);
    points->SetRadius(1.0);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(0, pointSource()->GetOutputPort());

    return mapper;
}
