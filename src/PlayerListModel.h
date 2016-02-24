#ifndef PLAYERLISTMODEL
#define PLAYERLISTMODEL

#include <QAbstractListModel>
#include <QListView>
#include <QVector>
#include <mutex>
#include "mclib/PlayerManager.h"

class PlayerListModel : public QAbstractListModel, public Minecraft::PlayerListener {
    Q_OBJECT

public:
    PlayerListModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void OnPlayerJoin(Minecraft::PlayerPtr player);
    void OnPlayerLeave(Minecraft::PlayerPtr player);
    void OnPlayerSpawn(Minecraft::PlayerPtr player);
    void OnPlayerDestroy(Minecraft::PlayerPtr player, Minecraft::EntityId eid);

private:
    void ForceRepaint();

    QVector<Minecraft::PlayerPtr> m_Players;
    mutable std::recursive_mutex m_Mutex;
};

#endif // PLAYERLISTMODEL

