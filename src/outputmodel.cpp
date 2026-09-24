#include "outputmodel.h"
#include <QtLogging>
#include <qscreen_platform.h>
// #include <QWaylandScreen> ?

XdgOutputManager::XdgOutputManager() : QWaylandClientExtensionTemplate(2) {}

XdgOutputManager::~XdgOutputManager() {
    if (this->isInitialized()) {
        this->destroy();
    }
}

XdgOutput::XdgOutput(::zxdg_output_v1* output, QScreen* screen) : zxdg_output_v1(output), screen(screen) {}

XdgOutput::~XdgOutput() {
    if (this->isInitialized()) {
        this->destroy();
    }
}

void XdgOutput::zxdg_output_v1_logical_position(int32_t x, int32_t y) {
    // qDebug() << this << "got logicalPosition" << QSize(x, y);
    this->logicalPosition = QSize(x, y);
}

void XdgOutput::zxdg_output_v1_logical_size(int32_t w, int32_t h) {
    // qDebug() << this << "got logicalSize" << QSize(w, h);
    this->logicalSize = QSize(w, h);
}

void XdgOutput::zxdg_output_v1_name(const QString& n) {
    // qDebug() << this << "got name" << n;
    this->name = n;
}

void XdgOutput::zxdg_output_v1_description(const QString& d) {
    // qDebug() << this << "got description" << d;
    this->description = d;
}

void XdgOutput::zxdg_output_v1_done() {
    if (this->onDone)
        this->onDone(this);
}

OutputModel::OutputModel(QObject* parent) : QAbstractListModel(parent) {
    QObject::connect(&m_manager, &QWaylandClientExtension::activeChanged, this, &OutputModel::onActiveChanged);
    QObject::connect(qGuiApp, &QGuiApplication::screenAdded, this, &OutputModel::addScreen);
    QObject::connect(qGuiApp, &QGuiApplication::screenRemoved, this, &OutputModel::removeScreen);
}

bool OutputModel::supported() const {
    return this->m_manager.isActive();
}

int OutputModel::rowCount(const QModelIndex& parent) const {
    return this->supported() ? int(this->m_outputs.size()) : 0;
}

QVariant OutputModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount())
        return {};

    auto o = m_outputs.at(index.row());
    switch (role) {
        case Roles::Name: return o->name;
        case Roles::Description: return o->description;
        case Roles::LogicalSize: return o->logicalSize;
        case Roles::LogicalPosition: return o->logicalPosition;
        default: return {};
    }
}

QHash<int, QByteArray> OutputModel::roleNames() const {
    return {
        {Roles::Name, "name"},
        {Roles::Description, "description"},
        {Roles::LogicalSize, "LogicalSize"},
        {Roles::LogicalPosition, "logicalPosition"},
    };
}

void OutputModel::addScreen(QScreen* screen) {
    if (!screen)
        return;
    // qDebug() << "screen added: " << screen->name();

    if (auto* waylandScreen = screen->nativeInterface<QNativeInterface::QWaylandScreen>()) {
        auto* wlOutput = waylandScreen->output();
        assert(wlOutput);

        auto* xdgOutput   = new XdgOutput(this->m_manager.get_xdg_output(wlOutput), screen);
        xdgOutput->onDone = [this](XdgOutput* o) {
            auto row = this->m_outputs.indexOf(o);
            if (row >= 0)
                emit this->dataChanged(index(row), index(row));
        };

        const int row = int(this->m_outputs.size());
        beginInsertRows({}, row, row);
        this->m_outputs.append(xdgOutput);
        endInsertRows();
    }
}

void OutputModel::removeScreen(QScreen* screen) {
    if (!screen)
        return;
    // qDebug() << "screen removed: " << screen->name();

    auto it = std::find_if(this->m_outputs.begin(), this->m_outputs.end(), [screen](XdgOutput* o) { return o->screen == screen; });
    if (it == this->m_outputs.end())
        return;

    const int row = int(it - this->m_outputs.begin());
    beginRemoveRows({}, row, row);
    this->m_outputs.erase(it);
    endRemoveRows();
}

void OutputModel::onActiveChanged() {
    // qDebug() << "activeChanged: " << this->supported();
    if (this->supported()) {
        for (QScreen* screen : QGuiApplication::screens()) {
            this->addScreen(screen);
        }
    }

    emit supportedChanged();
}
