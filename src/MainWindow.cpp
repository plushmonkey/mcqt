#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include "SettingsDialog.h"
#include <chrono>
#include <sstream>
#include "Settings.h"

s64 GetTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

ChatHandler::ChatHandler(mc::protocol::packets::PacketDispatcher* dispatcher, MainWindow *main)
    : mc::protocol::packets::PacketHandler(dispatcher), m_Main(main)
{
    using namespace mc::protocol;

    dispatcher->RegisterHandler(State::Login, login::Disconnect, this);
    dispatcher->RegisterHandler(State::Play, play::Chat, this);

    connect(this, SIGNAL(chatMessage(QString)), main, SLOT(appendChat(QString)));
}

ChatHandler::~ChatHandler() {
    GetDispatcher()->UnregisterHandler(this);
}

void ChatHandler::HandlePacket(mc::protocol::packets::in::DisconnectPacket* packet) {

    if (m_Main->GetPageIndex() != 1) return;

    QString reason = QString::fromStdWString(packet->GetReason());
    emit chatMessage(reason);
}

void ChatHandler::HandlePacket(mc::protocol::packets::in::ChatPacket* packet) {
    auto data = packet->GetChatData();
    std::string str = mc::util::ParseChatNode(data);

    str = mc::util::StripChatMessage(str);

    emit chatMessage(QString::fromStdString(str));
}

StatusHandler::StatusHandler(mc::protocol::packets::PacketDispatcher* dispatcher, MainWindow *main)
    : mc::protocol::packets::PacketHandler(dispatcher)
{
    using namespace mc::protocol;

    dispatcher->RegisterHandler(State::Login, login::Disconnect, this);

    connect(this, SIGNAL(statusUpdate(QString)), main, SLOT(updateStatus(QString)));
}

StatusHandler::~StatusHandler() {
    GetDispatcher()->UnregisterHandler(this);
}

void StatusHandler::HandlePacket(mc::protocol::packets::in::DisconnectPacket* packet) {
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
    m_Client = new mc::core::Client(&m_Dispatcher, mc::protocol::Version::Minecraft_1_10_2);
    m_Client->GetPlayerManager()->RegisterListener(&m_PlayerListModel);
    m_Client->GetConnection()->RegisterListener(this);

    m_ForgeHandler = std::make_shared<mc::util::ForgeHandler>(&m_Dispatcher, m_Client->GetConnection());

    m_ChatHandler = new ChatHandler(&m_Dispatcher, this);
    m_StatusHandler = new StatusHandler(&m_Dispatcher, this);

    ui->setupUi(this);

    this->setFont(Settings::GetInstance().GetFont());
    QString username = Settings::GetInstance().GetUsername();
    if (!username.isEmpty()) {
        ui->usernameEdit->setText(username);
        ui->passwordEdit->setFocus();
    }

    QString server = Settings::GetInstance().GetServer();
    if (!server.isEmpty())
        ui->serverEdit->setText(server);
    unsigned short port = Settings::GetInstance().GetPort();
    if (port)
        ui->portEdit->setText(QString::fromStdString(std::to_string(port)));

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

void MainWindow::OnSocketStateChange(mc::network::Socket::Status newStatus) {
    int page = ui->stackedWidget->currentIndex();

    if (newStatus == mc::network::Socket::Status::Disconnected && page == 1)
        emit appendChat("Disconnected from server.");
}

void MainWindow::OnLogin(bool success) {
    if (success) {
        mc::util::Yggdrasil* yggdrasil = m_Client->GetConnection()->GetYggdrasil();
        QString accessToken = QString::fromStdString(yggdrasil->GetAccessToken());
        QString clientToken = QString::fromStdString(yggdrasil->GetClientToken());

        std::string portStr = ui->portEdit->text().toStdString();
        u16 port = (u16)strtol(portStr.c_str(), nullptr, 10);

        Settings::GetInstance().SetAccessToken(accessToken);
        Settings::GetInstance().SetClientToken(clientToken);
        Settings::GetInstance().SetServer(ui->serverEdit->text());
        Settings::GetInstance().SetPort(port);

        emit changeStackedWidgetIndex(1);
        emit statusHide();
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
    Login();
}

void MainWindow::on_usernameEdit_returnPressed() {
    Login();
}

void MainWindow::on_passwordEdit_returnPressed() {
    Login();
}

void MainWindow::Login() {
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString host = ui->serverEdit->text();
    QString port = ui->portEdit->text();

    Settings::GetInstance().SetUsername(username);

    {
        mc::core::Client pingClient(&m_Dispatcher, mc::protocol::Version::Minecraft_1_10_2);
        try {
            pingClient.Ping(host.toStdString(), port.toShort());
        } catch (const std::exception& e) {
            ui->statusBar->showMessage(QString(e.what()));
            return;
        }

        u64 start = GetTime();

        while (!m_ForgeHandler->HasModInfo()) {
            if (GetTime() - start > 10000) {
                ui->statusBar->showMessage(QString("Timed out."));
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

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

    mc::core::Connection* connection = m_Client->GetConnection();

    mc::protocol::packets::out::ChatPacket packet(message);

    connection->SendPacket(&packet);

    ui->sendEdit->clear();
}
