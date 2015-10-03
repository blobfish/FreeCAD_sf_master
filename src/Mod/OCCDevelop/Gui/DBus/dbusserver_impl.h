#ifndef DBUSSERVER_IMPL
#define DBUSSERVER_IMPL

#include <QObject>

#include <TopAbs_ShapeEnum.hxx>

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
};


#endif
