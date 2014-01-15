/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <vector>
#include <sstream>
#include <Base/Console.h>
#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <Mod/Part/App/PartFeature.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Selection.h>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include "DisectBrep.h"
#include "VisualizePCurves.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdOCCDevelopTest THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD(CmdOCCDevelopTest);
CmdOCCDevelopTest::CmdOCCDevelopTest()
  :Command("OCCDevelop_Test")
{
    sAppModule    = "OCCDevelop";
    sGroup        = QT_TR_NOOP("OCCDevelop");
    sMenuText     = QT_TR_NOOP("Hello");
    sToolTipText  = QT_TR_NOOP("OCCDevelop Test function");
    sWhatsThis    = QT_TR_NOOP("OCCDevelop Test function");
    sStatusTip    = QT_TR_NOOP("OCCDevelop Test function");
    sPixmap       = "Test1";
    sAccel        = "CTRL+H";
}

void CmdOCCDevelopTest::activated(int iMsg)
{
    Base::Console().Message("Hello, World!\n");
}

DEF_STD_CMD(CmdOCCDevelopDisect);
CmdOCCDevelopDisect::CmdOCCDevelopDisect()
  :Command("OCCDevelop_Disect")
{
    sAppModule    = "OCCDevelop";
    sGroup        = QT_TR_NOOP("OCCDevelop");
    sMenuText     = QT_TR_NOOP("Disect");
    sToolTipText  = QT_TR_NOOP("OCCDevelop Disect function");
    sWhatsThis    = QT_TR_NOOP("OCCDevelop Disect function");
    sStatusTip    = QT_TR_NOOP("OCCDevelop Disect function");
    sPixmap       = "Test1";
//    sAccel        = "CTRL+H";
}

void CmdOCCDevelopDisect::activated(int iMsg)
{
    std::vector<Gui::SelectionSingleton::SelObj> objects = Gui::SelectionSingleton::instance().getSelection();
    if (objects.empty())
        return;

    Part::Feature *feature = dynamic_cast<Part::Feature *>(objects.at(0).pObject);
    if (!feature)
        return;

    TopoDS_Shape shape = feature->Shape.getValue();
    if (strlen(objects.at(0).SubName) > 0)
        shape = feature->Shape.getShape().getSubShape(objects.at(0).SubName);

    if (shape.IsNull())
        return;

    App::Document *currentDoc = App::GetApplication().getActiveDocument();
    if (!currentDoc)
        return;
    App::DocumentObjectGroup *group = static_cast<App::DocumentObjectGroup *>
            (currentDoc->addObject("App::DocumentObjectGroup", feature->getNameInDocument()));
    DisectBrep::disectBrepRecursive(group, shape, TopoDS_Shape(), 1);
}

DEF_STD_CMD(CmdOCCDevelopInfo);
CmdOCCDevelopInfo::CmdOCCDevelopInfo()
  :Command("OCCDevelop_Info")
{
    sAppModule    = "OCCDevelop";
    sGroup        = QT_TR_NOOP("OCCDevelop");
    sMenuText     = QT_TR_NOOP("Info");
    sToolTipText  = QT_TR_NOOP("OCCDevelop Info function");
    sWhatsThis    = QT_TR_NOOP("OCCDevelop Info function");
    sStatusTip    = QT_TR_NOOP("OCCDevelop Info function");
    sPixmap       = "Test1";
//    sAccel        = "CTRL+H";
}

void CmdOCCDevelopInfo::activated(int iMsg)
{
    std::vector<Gui::SelectionSingleton::SelObj> objects = Gui::SelectionSingleton::instance().getSelection();
    if (objects.empty())
        return;

    Part::Feature *feature = dynamic_cast<Part::Feature *>(objects.at(0).pObject);
    if (!feature)
        return;

    TopoDS_Shape shape = feature->Shape.getValue();
    if (strlen(objects.at(0).SubName) > 0)
        shape = feature->Shape.getShape().getSubShape(objects.at(0).SubName);

    if (shape.IsNull())
        return;

    if (shape.ShapeType() == TopAbs_FACE)
    {
        static std::vector<std::string> surfaceNames({"Plane", "Cylinder", "Cone", "Sphere", "Torus", "Bezier",
                                                      "BSpline", "Surface Of Revolution", "Surface Of Extrusion",
                                                      "Offset Surface", "Other Surface"});
        Handle(Geom_Surface) surface = BRep_Tool::Surface(TopoDS::Face(shape));
        GeomAdaptor_Surface surfaceTest(surface);
        Base::Console().Message(surfaceNames.at(static_cast<int>(surfaceTest.GetType())).c_str());
        Base::Console().Message("\n");
    }

}

DEF_STD_CMD(CmdOCCDevelopVisualizePCurves);
CmdOCCDevelopVisualizePCurves::CmdOCCDevelopVisualizePCurves()
  :Command("OCCDevelop_VisualizePCurves")
{
    sAppModule    = "OCCDevelop";
    sGroup        = QT_TR_NOOP("OCCDevelop");
    sMenuText     = QT_TR_NOOP("Visualize PCurves");
    sToolTipText  = QT_TR_NOOP("Visualize PCurves");
    sWhatsThis    = QT_TR_NOOP("Visualize PCurves");
    sStatusTip    = QT_TR_NOOP("Visualize PCurves");
    sPixmap       = "Test1";
//    sAccel        = "CTRL+H";
}

void CmdOCCDevelopVisualizePCurves::activated(int iMsg)
{
    std::vector<Gui::SelectionSingleton::SelObj> objects = Gui::SelectionSingleton::instance().getSelection();
    if (objects.empty())
        return;

    Part::Feature *feature = dynamic_cast<Part::Feature *>(objects.at(0).pObject);
    if (!feature)
        return;

    TopoDS_Shape shape = feature->Shape.getValue();
    if (strlen(objects.at(0).SubName) > 0)
        shape = feature->Shape.getShape().getSubShape(objects.at(0).SubName);

    if (shape.IsNull())
        return;

    if (shape.ShapeType() == TopAbs_FACE)
    {
        OCCDevelop::VisualizePCurves(TopoDS::Face(shape));
    }
}

void CreateOCCDevelopCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
    rcCmdMgr.addCommand(new CmdOCCDevelopTest());
    rcCmdMgr.addCommand(new CmdOCCDevelopDisect());
    rcCmdMgr.addCommand(new CmdOCCDevelopInfo());
    rcCmdMgr.addCommand(new CmdOCCDevelopVisualizePCurves());
}
