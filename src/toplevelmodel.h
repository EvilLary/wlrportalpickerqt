#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QtQmlIntegration>
#include <QtWaylandClient/QWaylandClientExtension>

#include "qwayland-ext-foreign-toplevel-list-v1.h"

class ToplevelHandle : public QtWayland::ext_foreign_toplevel_handle_v1 {
  public:
    explicit ToplevelHandle(::ext_foreign_toplevel_handle_v1* handle);

    QString                              title;
    QString                              appId;
    QString                              identifier;

    std::function<void(ToplevelHandle*)> onDone;
    std::function<void(ToplevelHandle*)> onClosed;

  protected:
    void ext_foreign_toplevel_handle_v1_closed() override;
    void ext_foreign_toplevel_handle_v1_done() override;
    void ext_foreign_toplevel_handle_v1_title(const QString& t) override;
    void ext_foreign_toplevel_handle_v1_app_id(const QString& id) override;
    void ext_foreign_toplevel_handle_v1_identifier(const QString& id) override;
};

class ExtTopLevelList : public QWaylandClientExtensionTemplate<ExtTopLevelList>, public QtWayland::ext_foreign_toplevel_list_v1 {
  public:
    explicit ExtTopLevelList();

    std::function<void(::ext_foreign_toplevel_handle_v1*)> onToplevel;

  protected:
    void ext_foreign_toplevel_list_v1_toplevel(::ext_foreign_toplevel_handle_v1* handle) override;
    void ext_foreign_toplevel_list_v1_finished() override;
};

class ToplevelModel : public QAbstractListModel {
    Q_OBJECT;
    QML_ELEMENT;
    QML_UNCREATABLE("pls no");

    Q_PROPERTY(bool supported READ supported NOTIFY supportedChanged);

  public:
    enum Roles {
        Title = Qt::UserRole + 1,
        AppId,
        Identifier,
    };

    explicit ToplevelModel(QObject* parent = nullptr);

    bool                   supported() const;
    int                    rowCount(const QModelIndex& parent = {}) const override;
    QVariant               data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    void supportedChanged();

  private:
    bool                   m_supported;
    void                   onToplevel(::ext_foreign_toplevel_handle_v1* handle);

    QList<ToplevelHandle*> m_toplevels;
    ExtTopLevelList        m_toplevelmanager;
};
