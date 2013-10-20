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
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkCutter.h>
#include <vtkDataSetReader.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkLookupTable.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlane.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkSmartPointer.h>


vtkSmartPointer<vtkActor> createOutline(vtkDataSetReader *reader);
vtkSmartPointer<vtkActor> createBoundaryWithColorMap(vtkDataSetReader *reader);
vtkSmartPointer<vtkActor> createIsosurfaces(vtkDataSetReader *reader);
vtkSmartPointer<vtkActor> createCutPlane(vtkDataSetReader *reader);

int main()
{
    vtkSmartPointer<vtkDataSetReader> reader = vtkDataSetReader::New();
    reader->SetFileName((common::dataPath() + "noise.vtk").c_str());

    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->SetBackground(0.2, 0.3, 0.4);

    renderer->AddActor(createOutline(reader));
    //renderer->AddActor(createBoundaryWithColorMap(reader));
    renderer->AddActor(createIsosurfaces(reader));
    renderer->AddActor(createCutPlane(reader));

    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
    window->AddRenderer(renderer);
    window->SetSize(800, 800);

    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    vtkSmartPointer<vtkInteractorStyleSwitch> interactorStyle =
        vtkInteractorStyleSwitch::New();
    interactor->SetInteractorStyle(interactorStyle);
    interactorStyle->SetCurrentStyleToTrackballCamera();
    interactor->Initialize();
    interactor->Start();
}

vtkSmartPointer<vtkActor> createOutline(vtkDataSetReader *reader)
{
    vtkSmartPointer<vtkOutlineFilter> outline = vtkOutlineFilter::New();
    outline->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(outline->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}

vtkSmartPointer<vtkActor> createBoundaryWithColorMap(vtkDataSetReader *reader)
{
    reader->Update();
    double range[2] = {reader->GetOutput()->GetScalarRange()[0],
                       reader->GetOutput()->GetScalarRange()[1]};

    vtkSmartPointer<vtkColorTransferFunction> transferFunction =
        vtkColorTransferFunction::New();
    transferFunction->AddRGBPoint(range[0], 1, 0, 0);
    transferFunction->AddRGBPoint(range[1], 0, 0, 1);

    vtkSmartPointer<vtkDataSetMapper> mapper = vtkDataSetMapper::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(transferFunction);

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}

vtkSmartPointer<vtkActor> createIsosurfaces(vtkDataSetReader *reader)
{
    vtkSmartPointer<vtkContourFilter> contour = vtkContourFilter::New();
    contour->SetInputConnection(reader->GetOutputPort());
    contour->SetNumberOfContours(3);
    contour->SetValue(0, 1.5);
    contour->SetValue(1, 3.0);
    contour->SetValue(2, 4.5);

    reader->Update();
    double range[2] = {reader->GetOutput()->GetScalarRange()[0],
                       reader->GetOutput()->GetScalarRange()[1]};

    vtkSmartPointer<vtkDataSetMapper> mapper = vtkDataSetMapper::New();
    mapper->SetInputConnection(contour->GetOutputPort());
    mapper->SetScalarRange(range);

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}

vtkSmartPointer<vtkActor> createCutPlane(vtkDataSetReader *reader)
{
    vtkSmartPointer<vtkCutter> cutter = vtkCutter::New();
    vtkSmartPointer<vtkPlane> plane = vtkPlane::New();
    plane->SetNormal(1, 1, 1);
    plane->SetOrigin(0, 0, 0);
    cutter->SetCutFunction(plane);
    cutter->SetInputConnection(reader->GetOutputPort());

    reader->Update();
    double range[2] = {reader->GetOutput()->GetScalarRange()[0],
                       reader->GetOutput()->GetScalarRange()[1]};
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkDataSetMapper::New();
    mapper->SetScalarRange(range);
    mapper->SetInputConnection(cutter->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}
