#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "SettingsDialog.h"

ChatHandler::ChatHandler(Minecraft::Packets::PacketDispatcher* dispatcher, MainWindow *main)
    : Minecraft::Packets::PacketHandler(dispatcher), m_Main(main)
{
    using namespace Minecraft::Protocol;

    dispatcher->RegisterHandler(State::Login, Login::Disconnect, this);
    dispatcher->RegisterHandler(State::Play, Play::Chat, this);

    connect(this, SIGNAL(chatMessage(QString)), main, SLOT(appendChat(QString)));
}

ChatHandler::~ChatHandler() {
    GetDispatcher()->UnregisterHandler(this);
}

void ChatHandler::HandlePacket(Minecraft::Packets::Inbound::DisconnectPacket* packet) {

    if (m_Main->GetPageIndex() != 1) return;

    QString reason = QString::fromStdWString(packet->GetReason());
    emit chatMessage(reason);
}

void ChatHandler::HandlePacket(Minecraft::Packets::Inbound::ChatPacket* packet) {
    auto data = packet->GetChatData();
    std::string str = ParseChatNode(data);

    str = StripChatMessage(str);

    emit chatMessage(QString::fromStdString(str));
}

StatusHandler::StatusHandler(Minecraft::Packets::PacketDispatcher* dispatcher, MainWindow *main)
    : Minecraft::Packets::PacketHandler(dispatcher)
{
    using namespace Minecraft::Protocol;

    dispatcher->RegisterHandler(State::Login, Login::Disconnect, this);

    connect(this, SIGNAL(statusUpdate(QString)), main, SLOT(updateStatus(QString)));
}

StatusHandler::~StatusHandler() {
    GetDispatcher()->UnregisterHandler(this);
}

void StatusHandler::HandlePacket(Minecraft::Packets::Inbound::DisconnectPacket* packet) {
    QString message = QString("Disconnected: ") + QString::fromStdWString(packet->GetReason());
    emit statusUpdate(message);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_Dispatcher(),
    m_PlayerListModel(this),
    m_Client(nullptr)
{
    m_Client = new Client(&m_Dispatcher);
    m_Client->GetPlayerManager()->RegisterListener(&m_PlayerListModel);
    m_Client->GetConnection()->RegisterListener(this);

    m_ChatHandler = new ChatHandler(&m_Dispatcher, this);
    m_StatusHandler = new StatusHandler(&m_Dispatcher, this);

    ui->setupUi(this);

    QList<int> sizes = {500, 150};
    ui->splitter->setSizes(sizes);
    ui->splitter->setStretchFactor(0, 5);
    ui->splitter->setStretchFactor(1, 0);

    ui->playersView->setModel(&m_PlayerListModel);

    connect(this, SIGNAL(changeStackedWidgetIndex(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(statusHide()), ui->statusBar, SLOT(hide()));
}

MainWindow::~MainWindow()
{
    m_Client->GetPlayerManager()->UnregisterListener(&m_PlayerListModel);
    m_Client->GetConnection()->UnregisterListener(this);
    delete m_ChatHandler;
    delete m_Client;
    delete ui;
}

int MainWindow::GetPageIndex() const {
    return ui->stackedWidget->currentIndex();
}

void MainWindow::OnSocketStateChange(Network::Socket::Status newStatus) {
    int page = ui->stackedWidget->currentIndex();

    if (newStatus == Network::Socket::Status::Disconnected && page == 1)
        emit appendChat("Disconnected from server.");
}

void MainWindow::OnLogin(bool success) {
    if (success) {
        emit changeStackedWidgetIndex(1);
        emit statusHide();
        //ui->statusBar->hide();
    } else {
        ui->statusBar->showMessage("Failed to login");
        m_Client->GetConnection()->Disconnect();
    }
}

void MainWindow::appendChat(QString str) {
    if (str.length() > 0)
        ui->chatEdit->append(str);
}

void MainWindow::updateStatus(QString str) {
    ui->statusBar->showMessage(str);
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString host = ui->serverEdit->text();
    QString port = ui->portEdit->text();

    try {
        m_Client->Login(host.toStdString(), port.toShort(), username.toStdString(), password.toStdString());
    } catch (const std::exception& e) {
        ui->statusBar->showMessage(QString(e.what()));
    }
}

void MainWindow::on_settingsButton_clicked() {
    SettingsDialog dialog(this);

    dialog.exec();
}

void MainWindow::on_sendEdit_returnPressed()
{
    std::wstring message = ui->sendEdit->text().toStdWString();

    if (message.length() == 0) return;

    Minecraft::Connection* connection = m_Client->GetConnection();

    Minecraft::Packets::Outbound::ChatPacket packet(message);

    connection->SendPacket(&packet);

    ui->sendEdit->clear();
}
