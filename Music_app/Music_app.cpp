#include "Music_app.h"
#include "addsongdialog.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QCloseEvent>
#include <QPushButton>
#include <algorithm>

Music_app::Music_app(QWidget* parent)
    : QMainWindow(parent)
    , m_modified(false)
{
    setWindowTitle(tr("Музыкальный архив"));
    setMinimumSize(760, 520);
    resize(960, 640);

    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    connectSignals();

    setModified(false);
    statusBar()->showMessage(tr("Готово"), 3000);
}

void Music_app::setupUi()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({ tr("Название"), tr("Автор"), tr("Год") });
    //делаем поведение колонок, чтобы название и автор тянулись на всё окно, а год поджимается под размер введенных нами цифр
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSortingEnabled(false);
    
    auto* btnAdd = new QPushButton(tr("Добавить"), this);
    auto* btnDelete = new QPushButton(tr("Удалить"), this);
    auto* btnInfo = new QPushButton(tr("Инфо"), this);

    auto* btnOldest = new QPushButton(tr("Найти старейшую"), this);

    connect(btnAdd, &QPushButton::clicked, this, &Music_app::onAddSong);
    connect(btnDelete, &QPushButton::clicked, this, &Music_app::onDeleteSong);
    connect(btnInfo, &QPushButton::clicked, this, &Music_app::onSongInfo);
    connect(btnOldest, &QPushButton::clicked, this, &Music_app::onFindOldest);

    auto* btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(0, 4, 0, 0);
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnDelete);
    btnLayout->addWidget(btnInfo);
    btnLayout->addStretch();         
    btnLayout->addWidget(btnOldest);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(m_table);
    mainLayout->addLayout(btnLayout);
}

void Music_app::setupMenuBar()
{
    m_actNew = new QAction(tr("&Новый"), this);
    m_actOpen = new QAction(tr("&Открыть..."), this);
    m_actSave = new QAction(tr("&Сохранить"), this);
    m_actSaveAs = new QAction(tr("Сохранить &как... "), this);

    m_actAdd = new QAction(tr("Добавить песню"), this);
    m_actDelete = new QAction(tr("Удалить песню"), this);
    m_actInfo = new QAction(tr("Информация о песне"), this);
    m_actFindOldest = new QAction(tr("Найти старейшую"), this);
    m_actSortAsc = new QAction(tr("Сортировка А→Я"), this);
    m_actSortDesc = new QAction(tr("Сортировка Я→А"), this);

    m_actAbout = new QAction(tr("О программе"), this);

    m_actNew->setShortcut(QKeySequence::New);
    m_actOpen->setShortcut(QKeySequence::Open);
    m_actSave->setShortcut(QKeySequence::Save);
    m_actSaveAs->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));

    QMenu* fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(m_actNew);
    fileMenu->addAction(m_actOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actSave);
    fileMenu->addAction(m_actSaveAs);

    menuBar()->addAction(m_actAbout);
}

void Music_app::setupToolBar()
{
    QToolBar* tb = addToolBar(tr("Сортировка"));
    tb->setMovable(false);
    tb->addAction(m_actSortAsc);
    tb->addAction(m_actSortDesc);
}

void Music_app::setupStatusBar()
{
    statusBar()->showMessage(tr("Готово"));
}

void Music_app::connectSignals()
{
    connect(m_actNew, &QAction::triggered, this, &Music_app::onNewFile);
    connect(m_actOpen, &QAction::triggered, this, &Music_app::onOpenFile);
    connect(m_actSave, &QAction::triggered, this, &Music_app::onSaveFile);
    connect(m_actSaveAs, &QAction::triggered, this, &Music_app::onSaveFileAs);

    connect(m_actAdd, &QAction::triggered, this, &Music_app::onAddSong);
    connect(m_actDelete, &QAction::triggered, this, &Music_app::onDeleteSong);
    connect(m_actInfo, &QAction::triggered, this, &Music_app::onSongInfo);

    connect(m_actFindOldest, &QAction::triggered, this, &Music_app::onFindOldest);
    connect(m_actSortAsc, &QAction::triggered, this, &Music_app::onSortAscending);
    connect(m_actSortDesc, &QAction::triggered, this, &Music_app::onSortDescending);

    connect(m_actAbout, &QAction::triggered, this, &Music_app::onAbout);

    connect(m_table, &QTableWidget::cellDoubleClicked,
        this, &Music_app::onRowDoubleClicked);
}

void Music_app::refreshTable()
{
    m_table->setRowCount(0);
    for (const Song& s : m_songs) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        populateRow(row, s);
    }
    statusBar()->showMessage(tr("Записей в архиве: %1").arg(m_songs.size()));
}

void Music_app::populateRow(int row, const Song& s)
{
    m_table->setItem(row, 0, new QTableWidgetItem(s.title));
    m_table->setItem(row, 1, new QTableWidgetItem(s.author));
    m_table->setItem(row, 2, new QTableWidgetItem(QString::number(s.year)));
}

int Music_app::selectedRow() const
{
    //currentRow() НЕ МЕНЯТЬ!!! иначе все упадет в момент, когда человеек тыкнет в пустую область таблицы
    return m_table->currentRow();
}

void Music_app::selectRow(int row)
{
    if (row >= 0 && row < m_table->rowCount())
        m_table->selectRow(row);
}

void Music_app::setModified(bool flag)
{
    m_modified = flag;
    QString title = tr("Музыкальный архив");
    if (!m_currentFile.isEmpty())
        title += QLatin1String(" - ") + QFileInfo(m_currentFile).fileName();
    if (m_modified)
        title += QLatin1String(" *");
    setWindowTitle(title);
}

bool Music_app::saveToFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Ошибка записи"),
            tr("Не удалось открыть файл для записи:\n%1").arg(path));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);

    out << FILE_MAGIC
        << FILE_VERSION
        << static_cast<qint32>(m_songs.size());

    // последовательная запись всех проектов song
    for (const Song& s : m_songs)
        out << s;

    file.close();
    return true;
}

bool Music_app::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Ошибка чтения"),
            tr("Не удалось открыть файл:\n%1").arg(path));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);

    quint32 magic = 0, version = 0;
    qint32  count = 0;
    in >> magic >> version >> count;

    if (in.status() != QDataStream::Ok || magic != FILE_MAGIC) {
        QMessageBox::warning(this, tr("Неверный формат"),
            tr("Файл повреждён или имеет неверный формат."));
        return false;
    }
    if (version > FILE_VERSION) {
        QMessageBox::warning(this, tr("Несовместимая версия"),
            tr("Файл создан более новой версией программы (версия %1).")
            .arg(version));
        return false;
    }
    if (count < 0 || count > 100000) {
        QMessageBox::warning(this, tr("Ошибка формата"),
            tr("Некорректное количество записей в файле: %1").arg(count));
        return false;
    }

    QVector<Song> songs;
    songs.reserve(count);
    for (qint32 i = 0; i < count; ++i) {
        Song s;
        in >> s;
        if (in.status() != QDataStream::Ok) {
            QMessageBox::warning(this, tr("Ошибка чтения"),
                tr("Файл повреждён: сбой при чтении записи %1.").arg(i + 1));
            return false;
        }
        songs.append(s);
    }

    m_songs = std::move(songs);
    return true;
}

bool Music_app::doSave(bool forceDialog)
{
    QString path = m_currentFile;

    if (forceDialog || path.isEmpty()) {
        QString startPath = path.isEmpty() ? tr("archive.msa") : path;

        path = QFileDialog::getSaveFileName(
            this,
            tr("Сохранить архив"),
            startPath,
            tr("Музыкальный архив (*.msa);;Все файлы (*)"));

        if (path.isEmpty())
            return false; // пользователь нажал отмену

        if (!path.endsWith(QLatin1String(".msa"), Qt::CaseInsensitive))
            path += QLatin1String(".msa");
    }

    if (!saveToFile(path))
        return false;

    m_currentFile = path;
    setModified(false);
    statusBar()->showMessage(tr("Сохранено: %1").arg(path), 5000);
    return true;
}

void Music_app::onNewFile()
{
    if (m_modified) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Новый архив"));
        msgBox.setText(tr("В архиве есть несохранённые изменения."));
        msgBox.setInformativeText(tr("Создать новый архив без сохранения?"));
        msgBox.setIcon(QMessageBox::Question);
        QPushButton* btnYes = msgBox.addButton(tr("Да, создать новый"), QMessageBox::AcceptRole);
        QPushButton* btnNo = msgBox.addButton(tr("Нет, отмена"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(btnNo);
        msgBox.exec();
        if (msgBox.clickedButton() != btnYes) return;
    }

    m_songs.clear();
    m_currentFile.clear();
    refreshTable();
    setModified(false);
    statusBar()->showMessage(tr("Новый архив создан"), 3000);
}

void Music_app::onOpenFile()
{
     if (m_modified) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Открытие файла"));
        msgBox.setText(tr("В архиве есть несохранённые изменения."));
        msgBox.setInformativeText(tr("Открыть другой файл без сохранения текущих изменений?"));
        msgBox.setIcon(QMessageBox::Question);
        QPushButton* btnYes = msgBox.addButton(tr("Да, открыть"), QMessageBox::AcceptRole);
        QPushButton* btnNo = msgBox.addButton(tr("Нет, отмена"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(btnNo);
        msgBox.exec();
        if (msgBox.clickedButton() != btnYes) return;
    }

    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Открыть архив"),
        QString(),
        tr("Музыкальный архив (*.msa);;Все файлы (*)"));

    if (path.isEmpty()) return;

    if (loadFromFile(path)) {
        m_currentFile = path;
        refreshTable();
        setModified(false);
        statusBar()->showMessage(
            tr("Открыт: %1 (%2 записей)").arg(path).arg(m_songs.size()), 5000);
    }
}

void Music_app::onSaveFile()
{
   doSave(false);
}

void Music_app::onSaveFileAs()
{
    doSave(true);
}

void Music_app::onAddSong()
{
    AddSongDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    Song s = dlg.getSong();
    m_songs.append(s);

    int row = m_table->rowCount();
    m_table->insertRow(row);
    populateRow(row, s);
    selectRow(row);

    setModified(true);
    statusBar()->showMessage(tr("Добавлена: «%1»").arg(s.title), 3000);
}

void Music_app::onDeleteSong()
{
    int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this,
            tr("Ничего не выбрано"),
            tr("Пожалуйста, выберите песню в таблице, которую хотите удалить."));
        return;
    }

    const Song& s = m_songs.at(row);
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Подтверждение удаления"));
    msgBox.setText(tr("Удалить песню «%1»?").arg(s.title));
    msgBox.setInformativeText(tr("Автор: %1, год: %2").arg(s.author, QString::number(s.year)));
    msgBox.setIcon(QMessageBox::Question);
    QPushButton* btnYes = msgBox.addButton(tr("Да, удалить"), QMessageBox::AcceptRole);
    QPushButton* btnNo = msgBox.addButton(tr("Нет, отмена"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(btnNo);
    msgBox.exec();
    if (msgBox.clickedButton() != btnYes) return;

    QString title = s.title;
    m_songs.remove(row);
    m_table->removeRow(row);

    setModified(true);
    statusBar()->showMessage(tr("Удалена: «%1»").arg(title), 3000);
}

void Music_app::onSongInfo()
{
    int row = selectedRow();
    if (row < 0) {
        QMessageBox::information(this,
            tr("Ничего не выбрано"),
            tr("Пожалуйста, выберите песню в таблице."));
        return;
    }

    const Song& s = m_songs.at(row);
    QMessageBox::information(
        this,
        tr("Информация о песне"),
        tr("<b>Название:</b> %1<br>"
            "<b>Автор:</b>    %2<br>"
            "<b>Год:</b>      %3")
        .arg(s.title.toHtmlEscaped(),
            s.author.toHtmlEscaped(),
            QString::number(s.year)));
}

void Music_app::onRowDoubleClicked(int /*row*/, int /*col*/)
{
    onSongInfo();
}

void Music_app::onFindOldest()
{
    if (m_songs.isEmpty()) {
        QMessageBox::information(this,
            tr("Архив пуст"),
            tr("В архиве нет ни одной записи. Добавьте песни и попробуйте снова."));
        return;
    }

    int minIdx = 0;
    for (int i = 1; i < m_songs.size(); ++i) {
        if (m_songs[i].year < m_songs[minIdx].year)
            minIdx = i;
    }

    selectRow(minIdx);
    const Song& s = m_songs.at(minIdx);

    QMessageBox::information(
        this,
        tr("Старейшая песня"),
        tr("Найдена старейшая песня в архиве:\n\n"
            "Название: %1\n"
            "Автор:    %2\n"
            "Год:      %3")
        .arg(s.title, s.author, QString::number(s.year)));
}

void Music_app::onSortAscending()
{
    if (m_songs.size() < 2) return;
    std::sort(m_songs.begin(), m_songs.end(),
        [](const Song& a, const Song& b) {
            return QString::localeAwareCompare(a.title, b.title) < 0;
        });
    refreshTable();
    setModified(true);
    statusBar()->showMessage(tr("Сортировка А→Я выполнена"), 3000);
}

void Music_app::onSortDescending()
{
    if (m_songs.size() < 2) return;
    std::sort(m_songs.begin(), m_songs.end(),
        [](const Song& a, const Song& b) {
            return QString::localeAwareCompare(a.title, b.title) > 0;
        });
    refreshTable();
    setModified(true);
    statusBar()->showMessage(tr("Сортировка Я→А выполнена"), 3000);
}

void Music_app::onAbout()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("О программе"));
    msgBox.setIcon(QMessageBox::NoIcon);

    msgBox.setText(tr("<h2>Музыкальный архив</h2>"));

    msgBox.setInformativeText(
        tr("<p>Версия 1.0</p>"
            "<p>Разработчик: один грустный и бедный студентик</p>"
            "<p>Заказчик: очень умный преподаватель</p>"
            "<p><i>P.s.: спасибо, что нажали на эту кнопочку!</i></p>"));

    QPushButton* btnOk = msgBox.addButton(
        tr("Хорошее приложение. Ставлю \"Отлично\""),
        QMessageBox::AcceptRole);
    msgBox.setDefaultButton(btnOk);

    msgBox.exec();
}

void Music_app::closeEvent(QCloseEvent* event)
{
    if (!m_modified) {
        event->accept();
        return;
    }
    // QMessageBox не менять, он гарантирует, что кнопки всегда будут на русском языке (я хотела все руссифицировать ахахах)
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Выход"));
    msgBox.setText(tr("В архиве есть несохранённые изменения."));
    msgBox.setInformativeText(tr("Сохранить изменения перед выходом?"));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton* btnSave = msgBox.addButton(tr("Сохранить"), QMessageBox::AcceptRole);
    QPushButton* btnDiscard = msgBox.addButton(tr("Не сохранять"), QMessageBox::DestructiveRole);
    QPushButton* btnCancel = msgBox.addButton(tr("Отмена"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(btnSave);

    msgBox.exec();

    if (msgBox.clickedButton() == btnCancel) {
        event->ignore();
        return;
    }

    if (msgBox.clickedButton() == btnSave) {
        bool saved = doSave(false);
        if (!saved) {
            event->ignore();
            return;
        }
    }

    event->accept();
}