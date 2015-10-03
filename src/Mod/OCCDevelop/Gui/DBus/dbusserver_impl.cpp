#include <Mod/OCCDevelop/Gui/PreCompiled.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Mod/Part/App/PartFeature.h>
#include <App/DocumentObjectGroup.h>
#include <Mod/OCCDevelop/Gui/DisectBrep.h>
#include <QDBusConnection>
#include <QDebug>
#include <sstream>
#include "dbusserver_impl.h"
#include "dbusserver.h"


static Part::TopoShape shapeFromString(QString data)
{
    std::string buffer = data.toStdString();
    std::istringstream stream;
    stream.str(buffer);

    Part::TopoShape shape;
    shape.importBrep(stream);
    return shape;
}

DBusServerImpl::DBusServerImpl(QObject *parent) : QObject(parent)
{

}

void DBusServerImpl::newDocument(const QString &docname)
{
    App::GetApplication().newDocument(docname.toLatin1().data());
}

void DBusServerImpl::startServer(QObject *parent)
{
    DBusServerImpl *server = new DBusServerImpl(parent);
    new FreecadDBusServer(server);

    QDBusConnection connection = QDBusConnection::sessionBus();
    bool ret = connection.registerService(QString::fromLatin1("org.freecad"));
    ret = connection.registerObject(QString::fromLatin1("/"), server);
}

void DBusServerImpl::addBrep(const QString &docname, const QString &objectname, const QString &data)
{
    Part::TopoShape shape = shapeFromString(data);

    App::Document *currentDoc;
    if (docname.isEmpty())
        currentDoc = App::GetApplication().getActiveDocument();
    else
        currentDoc = App::GetApplication().getDocument(docname.toLatin1().data());
    if (!currentDoc)
        return;

    Part::Feature *object = static_cast<Part::Feature *>
            (currentDoc->addObject("Part::Feature", objectname.toAscii().data()));
    if (!object)
        return;
    object->Shape.setValue(shape);
    currentDoc->recompute();
}

void DBusServerImpl::disectBrep(const QString &objectName, const QString &data)
{
    Part::TopoShape shape = shapeFromString(data);
    App::Document *currentDoc = App::GetApplication().getActiveDocument();
    if (!currentDoc)
        return;
    App::DocumentObjectGroup *group = static_cast<App::DocumentObjectGroup *>
            (currentDoc->addObject("App::DocumentObjectGroup", objectName.toAscii().data()));
    DisectBrep::disectBrepRecursive(group, shape._Shape, TopoDS_Shape(), 1);
}

