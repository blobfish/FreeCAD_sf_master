/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c FreecadDBusServer -a dbusserver.h:dbusserver.cpp org.freecad.xml
 *
 * qdbusxml2cpp is Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "dbusserver.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class FreecadDBusServer
 */

FreecadDBusServer::FreecadDBusServer(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

FreecadDBusServer::~FreecadDBusServer()
{
    // destructor
}

void FreecadDBusServer::addBrep(const QString &docname, const QString &objectname, const QString &data)
{
    // handle method call org.freecad.addBrep
    QMetaObject::invokeMethod(parent(), "addBrep", Q_ARG(QString, docname), Q_ARG(QString, objectname), Q_ARG(QString, data));
}

void FreecadDBusServer::newDocument(const QString &docname)
{
    // handle method call org.freecad.newDocument
    QMetaObject::invokeMethod(parent(), "newDocument", Q_ARG(QString, docname));
}

