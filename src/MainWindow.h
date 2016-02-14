#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mclib/Client.h"
#include "playerlistmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow;

class CircleMover : public QObject, public Minecraft::PlayerListener, public ClientListener {
    Q_OBJECT

signals:
    void chatMessage(QString str);

private:
    Minecraft::PlayerManager* m_PlayerManager;
    PlayerController* m_PlayerController;
    bool m_InWorld;
    s64 m_LastUpdate;
    s64 m_StartTime;
    Vector3d m_BasePosition;

public:
    CircleMover(Minecraft::PlayerManager* pm, PlayerController* pc, MainWindow* mw);

    ~CircleMover();

    void OnClientSpawn(Minecraft::PlayerPtr player);

    void OnTick();

    void Update();
};

class ChatHandler : public QObject, public Minecraft::Packets::PacketHandler {
    Q_OBJECT

public:
    ChatHandler(Minecraft::Packets::PacketDispatcher* dispatcher, MainWindow* main);
    ~ChatHandler();

    void HandlePacket(Minecraft::Packets::Inbound::ChatPacket* packet);
    void HandlePacket(Minecraft::Packets::Inbound::DisconnectPacket* packet);

signals:
    void chatMessage(QString message);
private:
    MainWindow* m_Main;
};

class StatusHandler : public QObject, public Minecraft::Packets::PacketHandler {
    Q_OBJECT

public:
    StatusHandler(Minecraft::Packets::PacketDispatcher* dispatcher, MainWindow* main);
    ~StatusHandler();

    void HandlePacket(Minecraft::Packets::Inbound::DisconnectPacket* packet);

signals:
    void statusUpdate(QString message);
};

class MainWindow : public QMainWindow, public Minecraft::ConnectionListener
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void OnSocketStateChange(Network::Socket::Status newStatus);
    void OnLogin(bool success);

    int GetPageIndex() const;

private slots:
    void on_loginButton_clicked();
    void on_settingsButton_clicked();

    void on_sendEdit_returnPressed();

signals:
    void changeStackedWidgetIndex(int index);
    void statusHide();

public slots:
    void appendChat(QString str);
    void updateStatus(QString str);

private:
    Client* m_Client;
    Minecraft::Packets::PacketDispatcher m_Dispatcher;
    PlayerListModel m_PlayerListModel;
    ChatHandler* m_ChatHandler;
    StatusHandler* m_StatusHandler;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
