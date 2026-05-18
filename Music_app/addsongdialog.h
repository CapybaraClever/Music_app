#ifndef ADDSONGDIALOG_H
#define ADDSONGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "song.h"

class AddSongDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSongDialog(QWidget* parent = nullptr);

    Song getSong() const;

private slots:
    void onAccept();

private:
    QLineEdit* m_titleEdit;
    QLineEdit* m_authorEdit;
    QSpinBox* m_yearSpin;
    QDialogButtonBox* m_buttons;
};

#endif