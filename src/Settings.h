#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFont>

class Settings {
private:
    QFont m_Font;
    QString m_Username;
    QString m_Server;
    unsigned short m_Port;
    QString m_AccessToken;
    QString m_ClientToken;

public:
    Settings();
    ~Settings();

    static Settings& GetInstance();

    bool Load(const QString& filename);
    bool Save(const QString& filename);

    QString GetUsername() const { return m_Username; }
    QString GetServer() const { return m_Server; }
    unsigned short GetPort() const { return m_Port; }
    QString GetAccessToken() const { return m_AccessToken; }
    QString GetClientToken() const { return m_ClientToken; }
    QFont GetFont() const { return m_Font; }

    void SetFont(const QFont& font) {
        m_Font = font;
        Save("settings.json");
    }

    void SetUsername(const QString& username) {
        m_Username = username;
        Save("settings.json");
    }

    void SetServer(const QString& server) {
        m_Server = server;
        Save("settings.json");
    }

    void SetPort(unsigned short port) {
        m_Port = port;
        Save("settings.json");
    }

    void SetAccessToken(const QString& accessToken) {
        m_AccessToken = accessToken;
        Save("settings.json");
    }

    void SetClientToken(const QString& clientToken) {
        m_ClientToken = clientToken;
        Save("settings.json");
    }
};

#endif // SETTINGS_H

