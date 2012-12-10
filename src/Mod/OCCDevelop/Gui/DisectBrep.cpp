#include "PreCompiled.h"
#include "DisectBrep.h"
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/PartFeature.h>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <QTextStream>
#include <BRepTools_WireExplorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

#include <QDebug>

void DisectBrep::disectBrepRecursive(App::DocumentObjectGroup *group, const TopoDS_Shape &shape,
                                     const TopoDS_Shape &parent, int count, TopAbs_ShapeEnum stop)
{
    if (shape.ShapeType() > stop)
        return;

    std::string gName = groupName(shape, count);
    App::DocumentObjectGroup *newGroup = group;
    if (stop != shape.ShapeType())
        newGroup = static_cast<App::DocumentObjectGroup *>
                (group->addObject("App::DocumentObjectGroup", gName.c_str()));

    std::ostringstream objectName;
    objectName << gName << orientText(shape.Orientation()) << "_";
    double tol = getTolerance(shape);
    if (tol != -1.0)
        objectName << tol << "___";
    if (shape.ShapeType() != TopAbs_COMPOUND && shape.ShapeType() != TopAbs_COMPSOLID)
    {
        Part::Feature *object = static_cast<Part::Feature *>
                (newGroup->addObject("Part::Feature", objectName.str().c_str()));
        object->Shape.setValue(shape);
    }

    if (shape.ShapeType() == TopAbs_EDGE && stop != TopAbs_EDGE)
    {
        TopoDS_Vertex vertex1 = TopExp::FirstVertex(TopoDS::Edge(shape));
        std::ostringstream vertex1Name;
        vertex1Name << "First Vertex " << orientText(vertex1.Orientation()) << "_" <<
                       BRep_Tool::Tolerance(vertex1) << "___";
        Part::Feature *object1 = static_cast<Part::Feature *>
                (newGroup->addObject("Part::Feature", vertex1Name.str().c_str()));
        object1->Shape.setValue(vertex1);

        TopoDS_Vertex vertex2 = TopExp::LastVertex(TopoDS::Edge(shape));
        std::ostringstream vertex2Name;
        vertex2Name << "Last Vertex " << orientText(vertex2.Orientation()) << "_" <<
                       BRep_Tool::Tolerance(vertex2) << "___";
        Part::Feature *object2 = static_cast<Part::Feature *>
                (newGroup->addObject("Part::Feature", vertex2Name.str().c_str()));
        object2->Shape.setValue(vertex2);

        return;
    }

    if (shape.ShapeType() == TopAbs_WIRE)
    {
        BRepTools_WireExplorer wireIt;
        int wireCount = 0;
        for (wireIt.Init(TopoDS::Wire(shape), TopoDS::Face(parent.Oriented(TopAbs_FORWARD))); wireIt.More(); wireIt.Next())
        {
            wireCount++;
            disectBrepRecursive(newGroup, wireIt.Current(), shape, wireCount, stop);
        }
        return;
    }

    TopoDS_Iterator it;
    int objectCount = 0;
    for (it.Initialize(shape, Standard_False); it.More(); it.Next())
    {
        objectCount++;
        disectBrepRecursive(newGroup, it.Value(), shape, objectCount, stop);
    }
}

std::string DisectBrep::groupName(const TopoDS_Shape &shape, int count)
{
    std::ostringstream nameStream;
    nameStream << shapeText(shape.ShapeType()) << ((count < 100) ? "0" : "") <<
                  ((count < 10) ? "0" : "") << count << "_";
    return nameStream.str();
}

double DisectBrep::getTolerance(const TopoDS_Shape &shape)
{
    double tolerance = -1.0;
    if (shape.ShapeType() == TopAbs_VERTEX)
    {
        TopoDS_Vertex vert = TopoDS::Vertex(shape);
        tolerance = BRep_Tool::Tolerance(vert);
    }
    if (shape.ShapeType() == TopAbs_EDGE)
    {
        TopoDS_Edge edge = TopoDS::Edge(shape);
        tolerance = BRep_Tool::Tolerance(edge);
    }
    if (shape.ShapeType() == TopAbs_FACE)
    {
        TopoDS_Face face = TopoDS::Face(shape);
        tolerance = BRep_Tool::Tolerance(face);
    }
    return tolerance;
}
