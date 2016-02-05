#include "playerlistmodel.h"

PlayerListModel::PlayerListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

void PlayerListModel::OnPlayerJoin(Minecraft::PlayerPtr player) {
    beginInsertRows(QModelIndex(), m_Players.size(), m_Players.size());
    m_Players.push_back(QString::fromStdWString(player->GetName()));
    endInsertRows();
}

void PlayerListModel::OnPlayerLeave(Minecraft::PlayerPtr player) {
    int index = m_Players.indexOf(QString::fromStdWString(player->GetName()));
    if (index == -1) return;

    beginRemoveRows(QModelIndex(), index, index);
    removeRow(index);
    m_Players.remove(index);
    endRemoveRows();
}

int PlayerListModel::rowCount(const QModelIndex &parent) const {
    return m_Players.count();
}

QVariant PlayerListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (index.row() >= m_Players.size()) return QVariant();

    if (role == Qt::DisplayRole)
        return m_Players.at(index.row());

    return QVariant();
}
