#ifndef DBUSSERVER_IMPL
#define DBUSSERVER_IMPL

#include <Mod/OCCDevelop/Gui/PreCompiled.h>
#include <QObject>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <Mod/Part/App/PartFeature.h>

class DBusServerImpl : public QObject
{
    Q_OBJECT
public:
    explicit DBusServerImpl(QObject *parent = 0);
    static void startServer(QObject *parent);

public Q_SLOTS:
    void addBrep(const QString &docname, const QString &objectname, const QString &data);
    void newDocument(const QString &docName);
    void disectBrep(const QString &objectName, const QString &data);

private:
    TopAbs_ShapeEnum nextSubType(TopAbs_ShapeEnum type);
    void dumpShapeRecursive(TopoDS_Shape shape, QString namePrefix, TopAbs_ShapeEnum stop = TopAbs_VERTEX);
    Part::TopoShape shapeFromString(QString data);
};


#endif
