#ifndef MUSIC_APP_H
#define MUSIC_APP_H

#include <QMainWindow>
#include <QTableWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QVector>

#include "song.h"

class Music_app : public QMainWindow
{
    Q_OBJECT

public:
    explicit Music_app(QWidget* parent = nullptr);
    ~Music_app() override = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:

    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();

    void onAddSong();
    void onDeleteSong();

    void onSongInfo();
    void onRowDoubleClicked(int row, int col);

    void onFindOldest();
    void onSortAscending();
    void onSortDescending();

    void onAbout();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void connectSignals();

    void refreshTable();
    void populateRow(int row, const Song& s);
    int  selectedRow() const;
    void selectRow(int row);
    void setModified(bool flag);

    bool saveToFile(const QString& path);
    bool loadFromFile(const QString& path);
    bool doSave(bool forceDialog);

    QVector<Song>  m_songs;
    QString        m_currentFile;
    bool           m_modified;

    QTableWidget* m_table;

    QAction* m_actNew;
    QAction* m_actOpen;
    QAction* m_actSave;
    QAction* m_actSaveAs;
    QAction* m_actAdd;
    QAction* m_actDelete;
    QAction* m_actInfo;
    QAction* m_actFindOldest;
    QAction* m_actSortAsc;
    QAction* m_actSortDesc;
    QAction* m_actAbout;

    static constexpr quint32 FILE_MAGIC = 0x4D534100;
    // Увеличили версию до 2, так как добавили новые данные в заголовок
    static constexpr quint32 FILE_VERSION = 2;
};

#endif