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

void PlayerListModel::OnPlayerSpawn(mc::core::PlayerPtr player) {
    ForceRepaint();
}

void PlayerListModel::OnPlayerDestroy(mc::core::PlayerPtr player, mc::EntityId eid) {
    ForceRepaint();
}

void PlayerListModel::OnPlayerJoin(mc::core::PlayerPtr player) {
    static const std::vector<std::wstring> ignoreNames = {
        L"?tab", L"BTLP Slot"
    };
    std::lock_guard<std::recursive_mutex> lock(m_Mutex);

    std::wstring name = player->GetName();
    for (std::wstring ignoreName : ignoreNames) {
        if (name.find(ignoreName) != std::string::npos || name.empty())
            return;
    }

    beginInsertRows(QModelIndex(), m_Players.size(), m_Players.size());
    m_Players.push_back(player);
    endInsertRows();
}

void PlayerListModel::OnPlayerLeave(mc::core::PlayerPtr player) {
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
        mc::core::PlayerPtr player = m_Players.at(index.row());

        if (role == Qt::DisplayRole) {
            const std::wstring& name = player->GetName();

            return QString::fromStdWString(name);
        }

        mc::entity::EntityPtr entity = player->GetEntity();
        if (entity) {
            if (role == Qt::BackgroundRole)
                return yellow;
            return blue;
        }
    }

    return QVariant();
}
