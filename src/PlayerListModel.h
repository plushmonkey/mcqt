#ifndef PLAYERLISTMODEL
#define PLAYERLISTMODEL

#include <QAbstractListModel>
#include <QVector>
#include "mclib/PlayerManager.h"

class PlayerListModel : public QAbstractListModel, public Minecraft::PlayerListener {
    Q_OBJECT

public:
    PlayerListModel(QObject* parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void OnPlayerJoin(Minecraft::PlayerPtr player);
    void OnPlayerLeave(Minecraft::PlayerPtr player);

private:
    QVector<QString> m_Players;
};

#endif // PLAYERLISTMODEL

