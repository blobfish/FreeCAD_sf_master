#ifndef DISECT_BREP_H
#define DISECT_BREP_H

#include <initializer_list>
#include <vector>
#include <string>
#include <QString>
#include <TopoDS_Shape.hxx>
#include <App/DocumentObjectGroup.h>

namespace DisectBrep
{
static std::string orientText(TopAbs_Orientation orient)
{
    static std::vector<std::string> names({"Forward", "Reversed", "Internal", "External"});
    return names.at(orient);
}

static std::string shapeText(TopAbs_ShapeEnum shapeType)
{
    static std::vector<std::string> names({"Compound", "CompSolid", "Solid", "Shell", "Face", "Wire", "Edge", "Vertex", "Shape"});
    return names.at(static_cast<int>(shapeType));
}

void disectBrepRecursive(App::DocumentObjectGroup *group, const TopoDS_Shape &shape, const TopoDS_Shape &parent,
                         int count, TopAbs_ShapeEnum stop = TopAbs_VERTEX);
std::string groupName(const TopoDS_Shape &shape, int count);
double getTolerance(const TopoDS_Shape &shape);
}
#endif
