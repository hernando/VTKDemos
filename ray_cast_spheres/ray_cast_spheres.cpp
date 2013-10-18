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
#include <vtkFloatArray.h>
#include <vtkOpenGLProperty.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkShader2.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2Collection.h>
#include <vtkSmartPointer.h>

#include <sstream>
#include <fstream>

#include <vtkOpenGLExtensionManager.h>
#include <GL/gl.h>

vtkSmartPointer<vtkPolyDataMapper> pointGrid();
vtkSmartPointer<vtkShaderProgram2> createSphereShadingProgram();

int main()
{

    vtkSmartPointer<vtkPolyDataMapper> mapper = pointGrid();

    /* Assinging the mapper to an actor */
    vtkSmartPointer<vtkActor> actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    /* Adding the ray casting shader to the actor */
    vtkSmartPointer<vtkOpenGLProperty> property =
        vtkOpenGLProperty::New();
    /* Enable shading in the property, otherwise shaders are not applied */
    property->ShadingOn();
    vtkSmartPointer<vtkShaderProgram2> program = createSphereShadingProgram();
    property->SetPropProgram(program);
    actor->SetProperty(property);

    /* Creating the renderer and the render window */
    vtkSmartPointer<vtkRenderer> renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    renderer->SetBackground(0.2, 0.3, 0.4);

    vtkSmartPointer<vtkRenderWindow> window = vtkRenderWindow::New();
    window->SetSize(800, 800);
    window->AddRenderer(renderer);

    /* Creating the interactor that handles the window events and provides
       the main rendering loop */
    vtkSmartPointer<vtkRenderWindowInteractor> interactor =
        vtkRenderWindowInteractor::New();

    interactor->SetRenderWindow(window);
    interactor->Initialize();
    interactor->Start();
}

vtkSmartPointer<vtkPolyDataMapper> pointGrid()
{
    /* Creating a poly data object of 10x10x10 points */
    vtkSmartPointer<vtkPolyData> data = vtkPolyData::New();
    size_t width = 5;
    size_t height = 5;
    size_t depth = 5;
    vtkSmartPointer<vtkPoints> vertices = vtkPoints::New();
    for (size_t i = 0; i < width; ++i)
        for (size_t j = 0; j < height; ++j)
            for (size_t k = 0; k < depth; ++k)
                vertices->InsertPoint(i * height * depth + j * depth + k,
                                      i * 10.0, j * 10.0, k * 10.0);
    data->SetPoints(vertices);

    vtkSmartPointer<vtkCellArray> points = vtkCellArray::New();
    for (vtkIdType i = 0; i < vtkIdType(width * height * depth); ++i)
        points->InsertNextCell(1, &i);
    data->SetVerts(points);

    /* Adding the radii of spheres as a point dataset */
    /* Creating the radii array */
    vtkSmartPointer<vtkFloatArray> radii =
        vtkSmartPointer<vtkFloatArray>::New();
    radii->SetName("radii");
    radii->SetNumberOfTuples(width * height * depth);
    for (size_t i = 0; i < width; ++i)
        for (size_t j = 0; j < height; ++j)
            for (size_t k = 0; k < depth; ++k)
            {
                float a = float(i) / (width - 1) - 0.5;
                float b = float(j) / (height - 1) - 0.5;
                float c = float(k) / (depth - 1) - 0.5;
                float radius = 8 - 5 * 2 * sqrt(a * a + b * b + c * c);
                radii->SetTuple(i * height * depth + j * depth + k, &radius);
            }

    data->GetPointData()->AddArray(radii);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
    /* Assigning the data object directly to the poly mapper */
    mapper->SetInputDataObject(data);
    mapper->MapDataArrayToVertexAttribute(
        "radiusAttrib", "radii", vtkDataObject::FIELD_ASSOCIATION_POINTS, -1);

    return mapper;
}

std::string readSource(const std::string &shaderFile);

vtkSmartPointer<vtkShaderProgram2> createSphereShadingProgram()
{
    vtkSmartPointer<vtkShaderProgram2> program =
        vtkShaderProgram2::New();
    std::string code;

    vtkSmartPointer<vtkShader2> vertex = vtkShader2::New();
    vertex->SetType(VTK_SHADER_TYPE_VERTEX);
    code = readSource("sphere.vert");
    vertex->SetSourceCode(code.c_str());
    program->GetShaders()->AddItem(vertex);

    vtkSmartPointer<vtkShader2> geometry = vtkShader2::New();
    geometry->SetType(VTK_SHADER_TYPE_GEOMETRY);
    code = readSource("sphere.geom");
    geometry->SetSourceCode(code.c_str());
    program->GetShaders()->AddItem(geometry);

    vtkSmartPointer<vtkShader2> fragment = vtkShader2::New();
    fragment->SetType(VTK_SHADER_TYPE_FRAGMENT);
    code = readSource("sphere.frag");
    fragment->SetSourceCode(code.c_str());
    program->GetShaders()->AddItem(fragment);

    program->SetGeometryTypeIn(VTK_GEOMETRY_SHADER_IN_TYPE_POINTS);
    program->SetGeometryTypeOut(VTK_GEOMETRY_SHADER_OUT_TYPE_TRIANGLE_STRIP);
    program->SetGeometryVerticesOut(4);

    return program;
}


std::string readSource(const std::string &shaderFile)
{
    std::string shader = common::shaderPath() + "/" + shaderFile;

    std::ifstream file(shader);
    if (file.fail())
    {
        std::cerr << "Couldn't find shader source: " << shader;
        return "";
    }


    std::stringstream out;
    /* Reading until a $ is found or EOF is reached. */
    do
    {
        int c = file.get();
        if (!file.eof())
        {
            if (file.fail())
            {
                std::cerr << "Error reading shader source: "
                          << shaderFile << std::endl;
                break;
            }
            out << (char)c;
        }
    }
    while (!file.eof());

    return out.str();
}
