#include "PlayerListModel.h"
#include <QColor>
#include "MainWindow.h"

PlayerListModel::PlayerListModel(QObject* parent)
    : QAbstractListModel(parent)
{

}

void PlayerListModel::ForceRepaint() {
    beginInsertRows(QModelIndex(), m_Players.size(), m_Players.size());
    endInsertRows();
}

void PlayerListModel::OnPlayerSpawn(Minecraft::PlayerPtr player) {
    ForceRepaint();
}

void PlayerListModel::OnPlayerDestroy(Minecraft::PlayerPtr player, Minecraft::EntityId eid) {
    ForceRepaint();
}

void PlayerListModel::OnPlayerJoin(Minecraft::PlayerPtr player) {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);

    beginInsertRows(QModelIndex(), m_Players.size(), m_Players.size());
    m_Players.push_back(player);
    endInsertRows();
}

void PlayerListModel::OnPlayerLeave(Minecraft::PlayerPtr player) {
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);

    int index = m_Players.indexOf(player);
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

    static QColor blue(31, 89, 195);
    static QColor yellow(255, 210, 100);

    if (role == Qt::DisplayRole || role == Qt::ForegroundRole || role == Qt::BackgroundRole) {
        Minecraft::PlayerPtr player = m_Players.at(index.row());

        if (role == Qt::DisplayRole) {
            const std::wstring& name = player->GetName();
            return QString::fromStdWString(name);
        }

        Minecraft::EntityPtr entity = player->GetEntity();
        if (entity) {
            if (role == Qt::BackgroundRole)
                return yellow;
            return blue;
        }
    }

    return QVariant();
}
