#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // QGuiApplication::setDesktopFileName("wlrportalpicker");
    QGuiApplication::setApplicationDisplayName("wlrportalpicker");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    engine.loadFromModule("WlrPortalPicker", "Main");

    return app.exec();
}
