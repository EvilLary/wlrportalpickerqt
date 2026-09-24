#pragma once

#include <toplevelmodel.h>
#include <outputmodel.h>

#include <QObject>
#include <QScreen>
#include <QtQmlIntegration>

class Backend : public QObject {
    Q_OBJECT;
    QML_ELEMENT;
    QML_SINGLETON;

  public:
    explicit Backend(QObject* parent = nullptr);

    Q_PROPERTY(ToplevelModel* toplevels READ toplevels CONSTANT);
    Q_PROPERTY(OutputModel* outputs READ outputs CONSTANT);

    Q_INVOKABLE void selectScreen(const QString& name);
    Q_INVOKABLE void selectWindow(const QString& id);

    ToplevelModel*   toplevels();
    OutputModel*     outputs();

  private:
    ToplevelModel m_toplevelmodel;
    OutputModel   m_outputmodel;
};
