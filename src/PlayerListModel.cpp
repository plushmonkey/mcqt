#include "playerlistmodel.h"

PlayerListModel::PlayerListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

void PlayerListModel::OnPlayerJoin(Minecraft::PlayerPtr player) {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);

    beginInsertRows(QModelIndex(), m_Players.size(), m_Players.size());
    m_Players.push_back(QString::fromStdWString(player->GetName()));
    endInsertRows();
}

void PlayerListModel::OnPlayerLeave(Minecraft::PlayerPtr player) {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);

    int index = m_Players.indexOf(QString::fromStdWString(player->GetName()));
    if (index == -1) return;

    beginRemoveRows(QModelIndex(), index, index);
    removeRow(index);
    m_Players.remove(index);
    endRemoveRows();
}

int PlayerListModel::rowCount(const QModelIndex &parent) const {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);
    return m_Players.count();
}

QVariant PlayerListModel::data(const QModelIndex &index, int role) const {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);
    if (!index.isValid()) return QVariant();
    if (index.row() >= m_Players.size()) return QVariant();

    if (role == Qt::DisplayRole)
        return m_Players.at(index.row());

    return QVariant();
}
