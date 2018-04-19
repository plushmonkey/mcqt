#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mclib/core/Client.h>
#include <mclib/util/VersionFetcher.h>
#include "PlayerListModel.h"
#include <QtConcurrent/QtConcurrent>

namespace Ui {
class MainWindow;
}

class MainWindow;

class ChatHandler : public QObject, public mc::protocol::packets::PacketHandler {
    Q_OBJECT

public:
    ChatHandler(mc::protocol::packets::PacketDispatcher* dispatcher, MainWindow* main);
    ~ChatHandler();

    void HandlePacket(mc::protocol::packets::in::ChatPacket* packet);
    void HandlePacket(mc::protocol::packets::in::DisconnectPacket* packet);

signals:
    void chatMessage(QString message);
private:
    MainWindow* m_Main;
};

class StatusHandler : public QObject, public mc::protocol::packets::PacketHandler {
    Q_OBJECT

public:
    StatusHandler(mc::protocol::packets::PacketDispatcher* dispatcher, MainWindow* main);
    ~StatusHandler();

    void HandlePacket(mc::protocol::packets::in::DisconnectPacket* packet);

signals:
    void statusUpdate(QString message);
};

class MainWindow : public QMainWindow, public mc::core::ConnectionListener
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void OnSocketStateChange(mc::network::Socket::Status newStatus);
    void OnLogin(bool success);

    int GetPageIndex() const;

private slots:
    void on_loginButton_clicked();
    void on_settingsButton_clicked();

    void on_usernameEdit_returnPressed();
    void on_passwordEdit_returnPressed();

    void on_sendEdit_returnPressed();

signals:
    void changeStackedWidgetIndex(int index);
    void statusHide();
    void enableLogin(bool enabled);
    void statusUpdate(QString message);

public slots:
    void appendChat(QString str);
    void updateStatus(QString str);

private:
    void Login();

    QFuture<void> m_PingWorker;
    mc::core::Client* m_Client;
    mc::protocol::packets::PacketDispatcher m_Dispatcher;
    std::unique_ptr<mc::util::VersionFetcher> m_VersionFetcher;
    PlayerListModel m_PlayerListModel;
    ChatHandler* m_ChatHandler;
    StatusHandler* m_StatusHandler;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
