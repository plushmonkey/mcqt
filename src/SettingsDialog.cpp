#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "MainWindow.h"
#include "Settings.h"
#include <QFontDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    DisplayFont();
}

void SettingsDialog::DisplayFont() {
    QWidget* parent = (QWidget*)this->parent();

    QString family = parent->font().family();
    int pointSize = parent->font().pointSize();
    ui->fontFamilyEdit->setText(family);
    ui->fontSizeEdit->setText(QString::fromStdString(std::to_string(pointSize)));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_fontButton_clicked()
{
    QWidget* parent = (QWidget*)this->parent();
    QString family = parent->font().family();
    int pointSize = parent->font().pointSize();

    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont(family, pointSize), this);
    if (ok) {
        MainWindow* mw = (MainWindow*)this->parent();

        Settings::GetInstance().SetFont(font);
        mw->setFont(font);
        this->setFont(font);
        DisplayFont();
    }
}
