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
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkInformation.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>


#include <typeinfo>
#include <unistd.h>

void addConeActor(vtkRenderer *renderer);
void addTetrahedron(vtkRenderer *renderer);
void loadPlyModel(vtkRenderer *renderer);
void addAnimation(vtkRenderer *renderer);

int main()
{
    /* Source data object that represents a sphere. */
    vtkSmartPointer<vtkSphereSource> sphere = vtkSphereSource::New();
    sphere->SetCenter(0, 0, 0);
    sphere->SetRadius(1);

    /* Mapper object that will convert the sphere source into polygonal
       mesh. */
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(sphere->GetOutputPort());
    //std::cout << typeid(*mapper).name() << std::endl;

    /* Creating the actor that will place the polygonal mesh into the
       scene and manage it's render attributes. */
    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    /* Creating the renderer and the render window */
    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.2, 0.3, 0.4);
    //std::cout << typeid(*renderer).name() << std::endl;

    //addConeActor(renderer);
    addTetrahedron(renderer);
    //loadPlyModel(renderer);

    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
    window->AddRenderer(renderer);
    window->SetSize(800, 800);

    /* Creating the interactor that handles the window events and provides
       the main rendering loop */
    //addAnimation(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    interactor->Initialize();
    interactor->Start();
}

void addConeActor(vtkRenderer *renderer)
{
    vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
    cone->SetResolution(64);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(cone->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetPosition(1.8, 0, 0);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0.5, 0);
    renderer->AddActor(actor);
}

void addTetrahedron(vtkRenderer *renderer)
{
    vtkSmartPointer<vtkPolyData> data = vtkPolyData::New();

    vtkSmartPointer<vtkPoints> vertices = vtkPoints::New();
    vertices->InsertPoint(0, 0.0, 0.0, sqrt(6.0)/3.0);
    vertices->InsertPoint(1, -0.5, sqrt(3.0)/4.0, 0.0);
    vertices->InsertPoint(2, 0.0, -sqrt(3.0)/4.0, 0.0);
    vertices->InsertPoint(3, 0.5, sqrt(3.0)/4.0, 0.0);
    data->SetPoints(vertices);

    vtkSmartPointer<vtkCellArray> faces = vtkCellArray::New();
    vtkIdType faceIndices[] = {0, 1, 2,
                               0, 2, 3,
                               0, 3, 1,
                               1, 3, 2};
    for (int i = 0; i < 12; i += 3)
        faces->InsertNextCell(3, faceIndices + i);
    data->SetPolys(faces);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputDataObject(0, data);

    //data->GetInformation()->Print(std::cout);
    //mapper->GetInputInformation()[0].Print(std::cout);

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetPosition(3.2, 0, 0);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 1, 0);

    renderer->AddActor(actor);
}

void loadPlyModel(vtkRenderer *renderer)
{
    vtkSmartPointer<vtkPLYReader> reader = vtkPLYReader::New();
    reader->SetFileName((common::dataPath() + "/bunny.ply").c_str());

    /* Smoothing the model. The smoothing simply computes per vertex normals
       and assigns them as attribute data to the vertices */
    vtkSmartPointer<vtkPolyDataNormals> filter = vtkPolyDataNormals::New();
    filter->ComputePointNormalsOn();
    filter->ComputeCellNormalsOff();
    filter->SetInputConnection(reader->GetOutputPort());
    filter->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(filter->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetPosition(5, -1, 0);
    actor->SetScale(10, 10, 10);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 1, 0.5);
    renderer->AddActor(actor);
}


class Animation : public vtkCommand
{
public:
    static Animation *New()
    {
        return new Animation;
    }

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkRenderer *renderer = static_cast<vtkRenderer*>(caller);
        renderer->GetActiveCamera()->Azimuth(1);
        renderer->GetRenderWindow()->Start();
        std::cout << "HELLO" << std::endl;
    }
};

void addAnimation(vtkRenderer *renderer)
{
    vtkSmartPointer<Animation> animation = Animation::New();
    renderer->AddObserver(vtkCommand::EndEvent, animation);
}
