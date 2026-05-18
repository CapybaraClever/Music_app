#include "addsongdialog.h"
#include <QMessageBox>

AddSongDialog::AddSongDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Добавить песню"));
    setMinimumWidth(340);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText(tr("Введите название..."));

    m_authorEdit = new QLineEdit(this);
    m_authorEdit->setPlaceholderText(tr("Введите автора..."));

    m_yearSpin = new QSpinBox(this);
    m_yearSpin->setRange(1000, 2100);
    m_yearSpin->setValue(2000);

    m_buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* formLayout = new QFormLayout;
    formLayout->addRow(tr("Название:"), m_titleEdit);
    formLayout->addRow(tr("Автор:"), m_authorEdit);
    formLayout->addRow(tr("Год:"), m_yearSpin);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttons);

    // тут поключаем кнопку "ОК" к кастомному слоту т.к. нужно валидировать ввод до закрытия диалога
    connect(m_buttons, &QDialogButtonBox::accepted, this, &AddSongDialog::onAccept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

Song AddSongDialog::getSong() const
{
    return Song(
        m_titleEdit->text().trimmed(),
        m_authorEdit->text().trimmed(),
        m_yearSpin->value()
    );
}

void AddSongDialog::onAccept()
{
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"),
            tr("Поле «Название» не может быть пустым."));
        m_titleEdit->setFocus();
        return;
    }
    if (m_authorEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"),
            tr("Поле «Автор» не может быть пустым."));
        m_authorEdit->setFocus();
        return;
    }

    accept();
}