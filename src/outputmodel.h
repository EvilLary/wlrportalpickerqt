#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QRect>
#include <QScreen>
#include <QVariant>
#include <QtQmlIntegration>
#include <QtWaylandClient/QWaylandClientExtension>

#include "qwayland-xdg-output-unstable-v1.h"

class XdgOutputManager : public QWaylandClientExtensionTemplate<XdgOutputManager>, public QtWayland::zxdg_output_manager_v1 {

  public:
    XdgOutputManager();
    ~XdgOutputManager() override;
};

class XdgOutput : public QtWayland::zxdg_output_v1 {
  public:
    XdgOutput(::zxdg_output_v1* output, QScreen* screen);
    ~XdgOutput();

    QString                         name;
    QString                         description;
    QSize                           logicalSize;
    QSize                           logicalPosition;
    QScreen*                        screen;

    std::function<void(XdgOutput*)> onDone;

  protected:
    void zxdg_output_v1_logical_position(int32_t x, int32_t y) override;
    void zxdg_output_v1_logical_size(int32_t w, int32_t h) override;
    void zxdg_output_v1_name(const QString& n) override;
    void zxdg_output_v1_description(const QString& d) override;
    void zxdg_output_v1_done() override;
};

class OutputModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("hf");

    Q_PROPERTY(bool supported READ supported NOTIFY supportedChanged);

  public:
    enum Roles {
        Name = Qt::UserRole + 1,
        Description,
        LogicalSize,
        LogicalPosition,
    };

    explicit OutputModel(QObject* parent = nullptr);

    bool                   supported() const;
    int                    rowCount(const QModelIndex& parent = {}) const override;
    QVariant               data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

  signals:
    void supportedChanged();

  public slots:
    void addScreen(QScreen* screen);
    void removeScreen(QScreen* screen);

  private:
    void              addXdgOutput(QScreen* screen);
    void              onActiveChanged();

    XdgOutputManager  m_manager;
    QList<XdgOutput*> m_outputs;
};
