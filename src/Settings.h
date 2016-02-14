#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFont>

class Settings {
private:
    QFont m_Font;
    QString m_Username;
    QString m_AccessToken;

public:
    Settings();
    ~Settings();

    static Settings& GetInstance();

    bool Load(const QString& filename);
    bool Save(const QString& filename);

    QString GetUsername() const { return m_Username; }
    QString GetAccessToken() const { return m_AccessToken; }
    QFont GetFont() const { return m_Font; }

    void SetFont(const QFont& font) {
        m_Font = font;
        Save("settings.json");
    }

    void SetUsername(const QString& username) {
        m_Username = username;
        Save("settings.json");
    }

    void SetAccessToken(const QString& accessToken) {
        m_AccessToken = accessToken;
        Save("settings.json");
    }

};

#endif // SETTINGS_H

