#include "Settings.h"
#include <QFile>
#include <json/json.h>
#include <QMessageBox>

namespace {
QString SettingsFilename = "settings.json";
}

Settings::Settings() {
    m_Font = QFont("Segoe UI", 10);
    Load(SettingsFilename);
}

Settings::~Settings() {
    Save(SettingsFilename);
}

Settings& Settings::GetInstance() {
    static Settings settings;
    return settings;
}

bool Settings::Load(const QString& filename) {
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString data = file.readAll();
    std::string str = data.toStdString();

    Json::Reader reader;
    Json::Value root;

    if (!reader.parse(str, root)) return false;

    QString family = QString::fromStdString(root["font"]["family"].asString());
    int pointSize = root["font"]["size"].asInt();

    m_Font = QFont(family, pointSize);

    if (!root["username"].isNull())
        m_Username = QString::fromStdString(root["username"].asString());
    if (!root["accessToken"].isNull())
        m_AccessToken = QString::fromStdString(root["accessToken"].asString());

    return true;
}

bool Settings::Save(const QString& filename) {
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    Json::Value root;

    root["font"]["family"] = m_Font.family().toStdString();
    root["font"]["size"] = m_Font.pointSize();

    if (!m_Username.isEmpty())
        root["username"] = m_Username.toStdString();
    if (!m_AccessToken.isEmpty())
        root["accessToken"] = m_AccessToken.toStdString();

    Json::StyledWriter writer;
    std::string prettyPrint = writer.write(root);

    file.write(prettyPrint.c_str());

    return true;
}
