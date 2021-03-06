#ifndef PLAYERLISTMODEL
#define PLAYERLISTMODEL

#include <QAbstractListModel>
#include <QListView>
#include <QVector>
#include <mutex>
#include <mclib/core/PlayerManager.h>

class PlayerListModel : public QAbstractListModel, public mc::core::PlayerListener {
    Q_OBJECT

public:
    PlayerListModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void OnPlayerJoin(mc::core::PlayerPtr player);
    void OnPlayerLeave(mc::core::PlayerPtr player);
    void OnPlayerSpawn(mc::core::PlayerPtr player);
    void OnPlayerDestroy(mc::core::PlayerPtr player, mc::EntityId eid);

private:
    void ForceRepaint();

    QVector<mc::core::PlayerPtr> m_Players;
    mutable std::recursive_mutex m_Mutex;
};

#endif // PLAYERLISTMODEL

