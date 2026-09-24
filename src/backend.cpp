#include "backend.h"
#include <QVariant>
#include <iostream>

Backend::Backend(QObject* parent) : QObject(parent) {}

// this is giga ass ik
// TODO: use xdg_output instead
void Backend::selectScreen(const QString& name) {
    std::cout << "Monitor: " << name.toStdString() << "\n";
    QGuiApplication::quit();
}

void Backend::selectWindow(const QString& id) {
    std::cout << "Window: " << id.toStdString() << "\n";
    QGuiApplication::quit();
}

ToplevelModel*   Backend::toplevels() {
    return &this->m_toplevelmodel;
}

OutputModel* Backend::outputs() {
    return &this->m_outputmodel;
}
