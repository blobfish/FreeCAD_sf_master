/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "PreCompiled.h"

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>

#include <Base/Console.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <Mod/Part/App/PartFeature.h>

#include "VisualizePCurves.h"

void OCCDevelop::VisualizePCurves(const TopoDS_Face& faceIn)
{
  App::Document *currentDoc = App::GetApplication().getActiveDocument();
  if (!currentDoc)
      return;
  
  App::DocumentObjectGroup *group = static_cast<App::DocumentObjectGroup *>
            (currentDoc->addObject("App::DocumentObjectGroup", "PCurves"));
  
  int edgeCount = 0;
  TopExp_Explorer faceIt;
  for (faceIt.Init(faceIn, TopAbs_EDGE); faceIt.More(); faceIt.Next())
  {
    edgeCount++;
    std::ostringstream stream;
    stream << "edge_" << ((edgeCount < 10) ? ("0") : ("")) << edgeCount;
    
    Standard_Real pFirst, pLast;
    Handle_Geom2d_Curve pCurve = BRep_Tool::CurveOnSurface(TopoDS::Edge(faceIt.Current()), faceIn, pFirst, pLast);
    
    TopoDS_Edge edgePCurve = BRepBuilderAPI_MakeEdge2d(pCurve, pFirst, pLast);
    
    Part::Feature *object = static_cast<Part::Feature *>
                (group->addObject("Part::Feature", stream.str().c_str()));
    object->Shape.setValue(edgePCurve);
  }
}
