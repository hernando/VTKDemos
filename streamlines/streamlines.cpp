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
#include <vtkDataSetReader.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkLookupTable.h>
#include <vtkOutlineFilter.h>
#include <vtkPlaneSource.h>
#include <vtkPlaneWidget.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRibbonFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRungeKutta4.h>
#include <vtkSmartPointer.h>
#include <vtkStreamLine.h>
#include <vtkTubeFilter.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

vtkSmartPointer<vtkImageData> readData(const std::string &filename);
vtkSmartPointer<vtkActor> createOutline(vtkDataSet *data);

class BeginInteraction : public vtkCommand
{
public:
    BeginInteraction(vtkActor *actor) : _actor(actor) {}

    virtual void Execute(vtkObject *, unsigned long, void*)
    {
        _actor->SetVisibility(false);
    }

    vtkActor *_actor;
};

class EndInteraction : public vtkCommand
{
public:
    EndInteraction(vtkPolyData *seeds, vtkActor *actor, vtkRenderWindow *window)
        : _seeds(seeds)
        , _actor(actor)
        , _window(window)
    {}

    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        _actor->SetVisibility(true);
        static_cast<vtkPlaneWidget*>(caller)->GetPolyData(_seeds);
        _window->Render();
    }

    vtkPolyData *_seeds;
    vtkActor *_actor;
    vtkRenderWindow *_window;
};

int main()
{
    vtkSmartPointer<vtkDataSet> data =
        readData((common::dataPath() + "/TwoSwirls_64x64x64.vec").c_str());

    /* Streamline seeder */
    //vtkSmartPointer<vtkPlaneSource> seeds = vtkPlaneSource::New();
    //seeds->SetXResolution(16);
    //seeds->SetYResolution(16);
    //seeds->SetOrigin(0, 16, 0);
    //seeds->SetPoint1(63.5, 32, 0.5);
    //seeds->SetPoint2(0.5, 32, 63.5);

    vtkSmartPointer<vtkPlaneWidget> widget = vtkPlaneWidget::New();
    vtkSmartPointer<vtkPolyData> seeds = vtkPolyData::New();
    widget->SetInputData(data);
    widget->PlaceWidget();
    widget->GetPolyData(seeds);

    /* Streamline filter */
    vtkSmartPointer<vtkStreamLine> streamLine = vtkStreamLine::New();
    streamLine->SetInputData(data);
    //streamLine->SetSourceConnection(seeds->GetOutputPort());
    streamLine->SetSourceData(seeds);
    streamLine->SetMaximumPropagationTime(200);
    streamLine->SetIntegrationStepLength(0.2);
    streamLine->SetStepLength(0.1);
    streamLine->SetIntegrationDirectionToForward();
    //streamLine->SetIntegrationDirectionToBoth();
    streamLine->SpeedScalarsOn();
    streamLine->VorticityOn();
    streamLine->SetIntegrator(vtkRungeKutta4::New());

    ///* Tube filter */
    //vtkSmartPointer<vtkTubeFilter> tubes = vtkTubeFilter::New();
    //tubes->SetRadius(0.25);
    //tubes->SetNumberOfSides(5);
    //tubes->SidesShareVerticesOn();
    //tubes->SetInputConnection(streamLine->GetOutputPort());

    /* Ribbon filter */
    vtkSmartPointer<vtkRibbonFilter> ribbons = vtkRibbonFilter::New();
    ribbons->SetWidth(0.25);
    ribbons->VaryWidthOn();
    ribbons->SetInputConnection(streamLine->GetOutputPort());

    vtkSmartPointer<vtkAlgorithm> paths = ribbons;

    /* Mapper and actor */
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(paths->GetOutputPort());
    double range[2] = {data->GetScalarRange()[0], data->GetScalarRange()[1]};
    mapper->SetScalarRange(range);
    vtkSmartPointer<vtkColorTransferFunction> transferFunction =
        vtkColorTransferFunction::New();
    transferFunction->AddRGBPoint(range[0], 1, 0, 0);
    transferFunction->AddRGBPoint(range[1], 0, 0, 1);
    mapper->SetLookupTable(transferFunction);

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    /* Creating the renderer and the render window */
    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    renderer->AddActor(createOutline(data));
    renderer->SetBackground(0.2, 0.3, 0.4);

    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
    window->AddRenderer(renderer);
    window->SetSize(800, 800);

    /* Creating the interactor that handles the window events and provides
       the main rendering loop */
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    vtkSmartPointer<vtkInteractorStyleSwitch> interactorStyle =
        vtkInteractorStyleSwitch::New();
    interactor->SetInteractorStyle(interactorStyle);
    interactorStyle->SetCurrentStyleToTrackballCamera();

    widget->SetInteractor(interactor);
    widget->SetResolution(16);
    widget->AddObserver("StartInteractionEvent",
                        new BeginInteraction(actor));
    widget->AddObserver("EndInteractionEvent",
                        new EndInteraction(seeds, actor, window));

    interactor->Initialize();
    interactor->Start();
}

vtkSmartPointer<vtkImageData> readData(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file)
        throw std::runtime_error("Could not open file " + filename);
    std::string s;
    std::getline(file, s);
    std::stringstream line(s);
    size_t x, y, z;
    line >> x >> y >> z;
    std::vector<float> data(x * y * z * 3);
    file.read((char*)&data[0], x * y * z * 3 * sizeof(float));

    if (file.fail())
        throw std::runtime_error("Error reading file " + filename);

    vtkSmartPointer<vtkImageData> field = vtkImageData::New();
    field->SetSpacing(1, 1, 1);
    field->SetDimensions(x, y, z);
    vtkSmartPointer<vtkDoubleArray> array = vtkDoubleArray::New();
    array->SetNumberOfComponents(3);
    array->SetNumberOfTuples(x * y * z);
    array->SetName("Velocity");
    for (size_t i = 0; i < x * y * z * 3; ++i)
        array->SetValue(i, data[i]);

    field->GetPointData()->SetVectors(array);

    return field;
}

vtkSmartPointer<vtkActor> createOutline(vtkDataSet *data)
{
    vtkSmartPointer<vtkOutlineFilter> outline = vtkOutlineFilter::New();
    outline->SetInputData(data);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(outline->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}
