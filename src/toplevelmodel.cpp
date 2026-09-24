#include <qwaylandclientextension.h>
#include <toplevelmodel.h>

#include <QtLogging>

ExtTopLevelList::ExtTopLevelList() : QWaylandClientExtensionTemplate(1) {}

void ExtTopLevelList::ext_foreign_toplevel_list_v1_toplevel(::ext_foreign_toplevel_handle_v1* handle) {
    this->onToplevel(handle);
}

void ExtTopLevelList::ext_foreign_toplevel_list_v1_finished() {
    if (this->isInitialized())
        this->destroy();
}

ToplevelHandle::ToplevelHandle(::ext_foreign_toplevel_handle_v1* handle) : QtWayland::ext_foreign_toplevel_handle_v1(handle) {}

void ToplevelHandle::ext_foreign_toplevel_handle_v1_closed() {
    // qDebug() << this << "closed";
    this->onClosed(this);
    if (this->isInitialized())
        this->destroy();
    delete this;
}

void ToplevelHandle::ext_foreign_toplevel_handle_v1_title(const QString& t) {
    // qDebug() << this << "got title" << t;
    this->title = t;
}

void ToplevelHandle::ext_foreign_toplevel_handle_v1_app_id(const QString& id) {
    // qDebug() << this << "got appid" << id;
    this->appId = id;
}

void ToplevelHandle::ext_foreign_toplevel_handle_v1_identifier(const QString& id) {
    // qDebug() << this << "got identifier" << id;
    this->identifier = id;
}

void ToplevelHandle::ext_foreign_toplevel_handle_v1_done() {
    // qDebug() << this << "got done";
    this->onDone(this);
}

ToplevelModel::ToplevelModel(QObject* parent) : QAbstractListModel(parent) {
    this->m_manager.onToplevel = [this](::ext_foreign_toplevel_handle_v1* handle) { this->onToplevel(handle); };

    QObject::connect(&this->m_manager, &QWaylandClientExtension::activeChanged, this, &ToplevelModel::supportedChanged);
}

bool ToplevelModel::supported() const {
    return this->m_manager.isActive();
}

void ToplevelModel::onToplevel(::ext_foreign_toplevel_handle_v1* handle) {
    // qDebug() << "new toplevel" << handle;

    auto toplevel = new ToplevelHandle(handle);

    toplevel->onDone = [this](ToplevelHandle* h) {
        const int row = this->m_toplevels.indexOf(h);
        if (row >= 0)
            emit this->dataChanged(index(row), index(row));
    };

    toplevel->onClosed = [this](ToplevelHandle* h) {
        QMetaObject::invokeMethod(
            this,
            [this, h] {
                const int row = this->m_toplevels.indexOf(h);
                if (row < 0)
                    return;
                beginRemoveRows({}, row, row);
                this->m_toplevels.removeAt(row);
                endRemoveRows();
            },
            Qt::QueuedConnection);
    };

    const int row = int(this->m_toplevels.size());
    beginInsertRows({}, row, row);
    this->m_toplevels.append(std::move(toplevel));
    endInsertRows();
}

int ToplevelModel::rowCount(const QModelIndex& parent) const {
    return int(this->m_toplevels.size());
}

QVariant ToplevelModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= rowCount())
        return {};

    auto t = this->m_toplevels.at(index.row());
    switch (role) {
        case Roles::Title: return t->title;
        case Roles::AppId: return t->appId;
        case Roles::Identifier: return t->identifier;
        default: return {};
    }
}

QHash<int, QByteArray> ToplevelModel::roleNames() const {
    return {
        {Roles::Title, "title"},
        {Roles::AppId, "appId"},
        {Roles::Identifier, "identifier"},
    };
}
